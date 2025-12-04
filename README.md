# File Inspector

## Overview

File Inspector is a simple graphical utility written in C using the GTK3 toolkit. It allows users to inspect files and directories, displaying detailed information such as file name, type, size, permissions, owner, group, last modification time, and full path. The application supports scanning individual files or recursively scanning entire directories.

## Features

-   **File and Directory Scanning**: Inspect details of a single file or recursively scan an entire directory.
-   **Detailed File Information**: Displays the following attributes for each entry:
    -   **Name**: The name of the file or directory.
    -   **Type**: File type (e.g., Regular file, Directory, Symbolic link).
    -   **Size**: Size in bytes.
    -   **Permissions**: Octal and symbolic representation (e.g., `drwxr-xr-x`).
    -   **Owner**: User owner.
    -   **Group**: Group owner.
    -   **MTime**: Last modification timestamp.
    -   **Path**: Full path to the file or directory.
-   **Recursive Scan Option**: A checkbox to toggle recursive scanning for directories.
-   **User-friendly Interface**: Built with GTK3 for a native look and feel.

## Technologies Used

-   **C Language**: The core logic of the application.
-   **GTK3**: A cross-platform widget toolkit for creating graphical user interfaces.

## Building the Application

To build File Inspector, you need to have a C compiler (like GCC) and the GTK3 development libraries installed on your system.

### Prerequisites

On Debian/Ubuntu-based systems:

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev
```

On Fedora/RHEL-based systems:

```bash
sudo dnf install gcc gtk3-devel
```

For other systems, please refer to the GTK3 documentation for installation instructions.

### Compilation

Navigate to the project root directory and run `make`:

```bash
make
```

This will compile the source files and create an executable named `fileinspector` in the root directory.

## Running the Application

After successful compilation, you can run the application from the project root:

```bash
./fileinspector
```

## Usage

1.  **Launch the application**: Execute `./fileinspector`.
2.  **Enter a path**: In the text entry field, type the path to a file or directory you wish to inspect, or click the "Chọn thư mục…" (Choose folder…) button to browse for a directory.
3.  **Recursive Scan**: Check the "Đệ quy" (Recursive) checkbox if you want to scan all subdirectories within a chosen directory.
4.  **Scan**: Click the "Quét" (Scan) button to initiate the scan.
5.  **View Results**: The results will be displayed in the table, showing detailed information for each file and directory found. The status bar at the bottom will provide feedback on the scan process.

## Screenshots

*(Currently, no screenshots are available. You might want to add some here to showcase the application's interface.)*

## Contributing

Feel free to fork the repository, open issues, or submit pull requests.

## License

This project is licensed under the MIT License. See the `LICENSE` file (if present) for details.