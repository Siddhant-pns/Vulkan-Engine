import os

# Folders to exclude from recursion
EXCLUDED_DIRS = {'build', '.git', '.github', '.vscode', 'external'}

def print_tree(start_path='.', prefix=''):
    try:
        entries = sorted(os.listdir(start_path))
    except PermissionError:
        return  # Skip folders we can't access

    entries = [e for e in entries if not (os.path.isdir(os.path.join(start_path, e)) and e in EXCLUDED_DIRS)]

    for index, entry in enumerate(entries):
        path = os.path.join(start_path, entry)
        connector = '└── ' if index == len(entries) - 1 else '├── '
        print(prefix + connector + entry)

        if os.path.isdir(path):
            extension = '    ' if index == len(entries) - 1 else '│   '
            print_tree(path, prefix + extension)

if __name__ == '__main__':
    print("Project Folder Tree Structure:\n")
    print_tree()
