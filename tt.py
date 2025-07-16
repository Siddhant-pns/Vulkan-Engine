import os
from pathlib import Path

# Folders to search
search_dirs = [ "plugins"]

# Output file
output_file = "vulkan_files.txt"

def list_all_files(root_dirs):
    """Recursively lists all files in given root_dirs with their modification time."""
    all_files = []
    for root_dir in root_dirs:
        if not os.path.exists(root_dir):
            print(f"Skipping missing folder: {root_dir}")
            continue
        for dirpath, dirnames, filenames in os.walk(root_dir):
            for filename in filenames:
                file_path = os.path.join(dirpath, filename)
                # Get modification time for sorting
                mod_time = os.path.getmtime(file_path)
                all_files.append((mod_time, file_path))
    # Sort by modification time (earliest first)
    all_files.sort()
    return all_files

def append_file_content(output, file_path, heading):
    """Appends the content of file_path to output file with heading and page break."""
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
        output.write(f"=== {heading} ===\n\n")
        output.write(content)
        output.write("\n\n\f\n\n")  # Page break
    except Exception as e:
        print(f"Error reading {file_path}: {e}")

def main():
    project_root = Path(__file__).parent.resolve()
    os.chdir(project_root)  # Ensure we're in the project root
    print(f"Searching all files in: {', '.join(search_dirs)}")

    all_files = list_all_files(search_dirs)
    print(f"Total files found: {len(all_files)}")

    with open(output_file, "w", encoding="utf-8") as out:
        for mod_time, file_path in all_files:
            print(f"Processing: {file_path}")
            relative_path = os.path.relpath(file_path, project_root)
            append_file_content(out, file_path, relative_path)

    print("\nDone!")
    print(f"All files from {search_dirs} written to: {output_file}")

if __name__ == "__main__":
    main()
