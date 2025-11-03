#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <time.h>

/* Cột dùng chung giữa UI và utils */
typedef enum {
    COL_NAME = 0,
    COL_TYPE,
    COL_SIZE,
    COL_PERM,
    COL_OWNER,
    COL_GROUP,
    COL_MTIME,
    COL_PATH,
    N_COLS
} Columns;

/* Helpers chuyển đổi thuộc tính */
const char* file_type_string(mode_t m);
void perm_to_string(mode_t m, char *out /* >= 11 bytes */);
void time_to_string(time_t t, char *buf, size_t n);

/* Ghi thông tin 1 đường dẫn (file hoặc link) vào GtkListStore */
gboolean stat_path_to_store(GtkListStore *store, const char *path, GError **err);

/* Duyệt thư mục dirpath, đổ dữ liệu vào GtkListStore (lstat, có đệ quy) */
gboolean scan_dir_to_store(GtkListStore *store,
                           const char *dirpath,
                           gboolean recursive,
                           GError **err);

#endif /* FILE_UTILS_H */

