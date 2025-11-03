#define _XOPEN_SOURCE 700
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "../include/file_utils.h"

typedef struct {
    GtkWidget    *entry_path;
    GtkWidget    *check_recursive;
    GtkListStore *store;
    GtkWidget    *statusbar;
    guint         status_ctx;
} AppWidgets;

/* ----------------- UI: tạo TreeView + model ----------------- */
static GtkWidget* create_tree_view(GtkListStore **out_store) {
    GtkListStore *store = gtk_list_store_new(N_COLS,
        G_TYPE_STRING,  // name
        G_TYPE_STRING,  // type
        G_TYPE_INT64,   // size
        G_TYPE_STRING,  // perm
        G_TYPE_STRING,  // owner
        G_TYPE_STRING,  // group
        G_TYPE_STRING,  // mtime
        G_TYPE_STRING   // path
    );

    GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(view), TRUE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view), TRUE);

    struct { const char* title; int col; gboolean numeric; } cols[] = {
        {"Name",  COL_NAME,  FALSE},
        {"Type",  COL_TYPE,  FALSE},
        {"Size",  COL_SIZE,  TRUE},
        {"Perm",  COL_PERM,  FALSE},
        {"Owner", COL_OWNER, FALSE},
        {"Group", COL_GROUP, FALSE},
        {"MTime", COL_MTIME, FALSE},
        {"Path",  COL_PATH,  FALSE}
    };

    for (guint i = 0; i < G_N_ELEMENTS(cols); ++i) {
        GtkCellRenderer *rend = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
            cols[i].title, rend, "text", cols[i].col, NULL);
        if (cols[i].numeric) {
            gtk_tree_view_column_set_alignment(col, 1.0);
            g_object_set(rend, "xalign", 1.0, NULL);
        }
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    }

    if (out_store) *out_store = store; // giữ 1 ref cho AppWidgets
    return view;
}

/* ----------------- Callbacks ----------------- */

static void on_choose_folder(GtkButton *btn, gpointer user_data) {
    AppWidgets *app = (AppWidgets*) user_data;
    GtkWidget *dlg = gtk_file_chooser_dialog_new(
        "Chọn thư mục",
        NULL,
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Hủy", GTK_RESPONSE_CANCEL,
        "_Chọn", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        char *folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        gtk_entry_set_text(GTK_ENTRY(app->entry_path), folder);
        g_free(folder);
    }
    gtk_widget_destroy(dlg);
}

static void on_scan(GtkButton *btn, gpointer user_data) {
    AppWidgets *app = (AppWidgets*) user_data;
    const char *path = gtk_entry_get_text(GTK_ENTRY(app->entry_path));
    gboolean recursive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->check_recursive));

    if (!path || !*path) {
        gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), app->status_ctx,
                           "Vui lòng nhập/chọn đường dẫn.");
        return;
    }

    gtk_list_store_clear(app->store);

    struct stat st;
    if (lstat(path, &st) == -1) {
        gchar *msg = g_strdup_printf("Không thể truy cập: %s (%s)", path, g_strerror(errno));
        gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), app->status_ctx, msg);
        g_free(msg);
        return;
    }

    GError *err = NULL;
    gboolean ok = FALSE;

    if (S_ISDIR(st.st_mode)) {
        ok = scan_dir_to_store(app->store, path, recursive, &err);
    } else {
        ok = stat_path_to_store(app->store, path, &err);
    }

    if (!ok) {
        gchar *msg = g_strdup_printf("Lỗi: %s", err ? err->message : "Không rõ");
        gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), app->status_ctx, msg);
        g_clear_error(&err);
        g_free(msg);
        return;
    }

    gchar *done = g_strdup_printf("Hoàn tất quét: %s%s", path, recursive ? " (đệ quy)" : "");
    gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), app->status_ctx, done);
    g_free(done);
}

/* ----------------- App bootstrap ----------------- */

static void activate(GtkApplication* app, gpointer user_data) {
    AppWidgets *aw = g_new0(AppWidgets, 1);

    GtkWidget *win = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(win), "File Inspector (GTK3)");
    gtk_window_set_default_size(GTK_WINDOW(win), 1100, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    // Top bar
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);

    GtkWidget *btn_choose = gtk_button_new_with_label("Chọn thư mục…");
    gtk_box_pack_start(GTK_BOX(hbox), btn_choose, FALSE, FALSE, 0);

    aw->entry_path = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(aw->entry_path),
        "/home/you/Documents hoặc đường dẫn file…");
    gtk_box_pack_start(GTK_BOX(hbox), aw->entry_path, TRUE, TRUE, 0);

    aw->check_recursive = gtk_check_button_new_with_label("Đệ quy");
    gtk_box_pack_start(GTK_BOX(hbox), aw->check_recursive, FALSE, FALSE, 0);

    GtkWidget *btn_scan = gtk_button_new_with_label("Quét");
    gtk_box_pack_start(GTK_BOX(hbox), btn_scan, FALSE, FALSE, 0);

    // Tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    GtkWidget *tree = create_tree_view(&aw->store);
    gtk_container_add(GTK_CONTAINER(scrolled), tree);

    // Statusbar
    aw->statusbar = gtk_statusbar_new();
    aw->status_ctx = gtk_statusbar_get_context_id(GTK_STATUSBAR(aw->statusbar), "status");
    gtk_box_pack_start(GTK_BOX(vbox), aw->statusbar, FALSE, FALSE, 0);

    // Signals
    g_signal_connect(btn_choose, "clicked", G_CALLBACK(on_choose_folder), aw);
    g_signal_connect(btn_scan,   "clicked", G_CALLBACK(on_scan), aw);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(win);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.fileinspector",
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

