import os
from pathlib import Path

# List of file names to search for
files_to_find = [
    "main.cpp",
    "Application.cpp", "Application.h",
    "Window.cpp", "Window.h",
    "Input.cpp", "Input.h",
    "RenderAPI.cpp", "RenderAPI.h",
    "IRenderBackend.h",
    "Camera.cpp", "Camera.h",
    "Mesh.cpp", "Mesh.h"
]

# Root directories to search within
search_dirs = ["engine", "plugins"]

# Output file
output_file = "collected_files.txt"

def find_first_occurrence(root_dirs, target_file):
    """Finds the first occurrence of target_file in the given root_dirs."""
    for root_dir in root_dirs:
        for dirpath, dirnames, filenames in os.walk(root_dir):
            if target_file in filenames:
                return os.path.join(dirpath, target_file)
    return None

def append_file_content(output, file_path, heading):
    """Appends the content of file_path to output file with heading and page break."""
    with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    output.write(f"=== {heading} ===\n\n")
    output.write(content)
    output.write("\n\n\f\n\n")  # Page break

def main():
    project_root = Path(__file__).parent.resolve()
    os.chdir(project_root)  # Make sure we're in the project root
    print(f"Searching in project root: {project_root}")

    found_files = []
    with open(output_file, "w", encoding="utf-8") as out:
        for filename in files_to_find:
            print(f"Searching for {filename}...")
            file_path = find_first_occurrence(search_dirs, filename)
            if file_path:
                print(f"Found: {file_path}")
                append_file_content(out, file_path, filename)
                found_files.append(file_path)
            else:
                print(f"Not found: {filename}")

    print("\nDone!")
    print(f"Collected content written to: {output_file}")
    print(f"Total files found: {len(found_files)} of {len(files_to_find)}")

if __name__ == "__main__":
    main()
