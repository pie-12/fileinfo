#define _XOPEN_SOURCE 700
#include "../include/file_utils.h"

#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/* ----------------- Helpers dùng nội bộ ----------------- */
static void add_row(GtkListStore *store,
                    const char *name,
                    const char *type,
                    long long size,
                    const char *perm,
                    const char *owner,
                    const char *group,
                    const char *mtime,
                    const char *path)
{
    GtkTreeIter it;
    gtk_list_store_append(store, &it);
    gtk_list_store_set(store, &it,
        COL_NAME,  name,
        COL_TYPE,  type,
        COL_SIZE,  (gint64) size,
        COL_PERM,  perm,
        COL_OWNER, owner,
        COL_GROUP, group,
        COL_MTIME, mtime,
        COL_PATH,  path,
        -1);
}

/* ----------------- API triển khai ----------------- */

const char* file_type_string(mode_t m) {
    if (S_ISREG(m))  return "FILE";
    if (S_ISDIR(m))  return "DIR";
    if (S_ISLNK(m))  return "LINK";
    if (S_ISCHR(m))  return "CHR";
    if (S_ISBLK(m))  return "BLK";
    if (S_ISFIFO(m)) return "FIFO";
    if (S_ISSOCK(m)) return "SOCK";
    return "UNKNOWN";
}

void perm_to_string(mode_t m, char *out /* >= 11 bytes */) {
    out[0] = S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : S_ISCHR(m) ? 'c' :
             S_ISBLK(m) ? 'b' : S_ISFIFO(m) ? 'p' : S_ISSOCK(m) ? 's' : '-';
    out[1] = (m & S_IRUSR) ? 'r' : '-';
    out[2] = (m & S_IWUSR) ? 'w' : '-';
    out[3] = (m & S_IXUSR) ? 'x' : '-';
    out[4] = (m & S_IRGRP) ? 'r' : '-';
    out[5] = (m & S_IWGRP) ? 'w' : '-';
    out[6] = (m & S_IXGRP) ? 'x' : '-';
    out[7] = (m & S_IROTH) ? 'r' : '-';
    out[8] = (m & S_IWOTH) ? 'w' : '-';
    out[9] = (m & S_IXOTH) ? 'x' : '-';
    out[10] = '\0';
}

void time_to_string(time_t t, char *buf, size_t n) {
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &lt);
}

gboolean stat_path_to_store(GtkListStore *store, const char *path, GError **err) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        if (err) g_set_error(err, g_quark_from_static_string("file-utils"),
                             errno, "lstat failed on %s: %s", path, g_strerror(errno));
        return FALSE;
    }

    char perms[11]; perm_to_string(st.st_mode, perms);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    const char *owner = pw ? pw->pw_name : "?";
    const char *group = gr ? gr->gr_name : "?";

    char tbuf[64]; time_to_string(st.st_mtime, tbuf, sizeof tbuf);

    gchar *base = g_path_get_basename(path);
    add_row(store, base, file_type_string(st.st_mode),
            (long long) st.st_size, perms, owner, group, tbuf, path);
    g_free(base);

    return TRUE;
}

gboolean scan_dir_to_store(GtkListStore *store,
                           const char *dirpath,
                           gboolean recursive,
                           GError **err)
{
    DIR *dir = opendir(dirpath);
    if (!dir) {
        if (err) g_set_error(err, g_quark_from_static_string("file-utils"),
                             errno, "opendir failed on %s: %s", dirpath, g_strerror(errno));
        return FALSE;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (g_strcmp0(ent->d_name, ".") == 0 || g_strcmp0(ent->d_name, "..") == 0)
            continue;

        char full[4096];
        g_snprintf(full, sizeof(full), "%s/%s", dirpath, ent->d_name);

        struct stat st;
        if (lstat(full, &st) == -1) {
            /* skip lỗi từng mục để tiếp tục quét */
            continue;
        }

        char perms[11]; perm_to_string(st.st_mode, perms);
        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        const char *owner = pw ? pw->pw_name : "?";
        const char *group = gr ? gr->gr_name : "?";
        char tbuf[64]; time_to_string(st.st_mtime, tbuf, sizeof tbuf);

        add_row(store, ent->d_name, file_type_string(st.st_mode),
                (long long) st.st_size, perms, owner, group, tbuf, full);

        if (recursive && S_ISDIR(st.st_mode)) {
            /* đệ quy thư mục con */
            scan_dir_to_store(store, full, TRUE, NULL);
        }
    }

    closedir(dir);
    return TRUE;
}

