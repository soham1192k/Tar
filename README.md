# Tar
A simple and lightweight implementation of the tar utility, using C.

## Functionalities:
- Creation : Use the -c flag.
- Extraction : Use the -d flag.
- Extraction of a single file : Use the -e flag.
- List tar info : Use the -l flag.

## How to run(Using makefile):
- Add the desired test case in "Test".
- Modify the makefile accordingly.
- Run `make`.
- Run the `run_test.sh`script. If after extraction, the file contents match the original file contents, we get a success message on the console.

## How to run(Terminal):
- `./myTar -c <path of a directory> <tar filename>`
- `./myTar -d <tar filepath>`
- `./myTar -e <tar filepath> <single filename>`
- `./myTar -l <tar filepath>`

### Assumptions:
- There will be a maximum of 30000 files.
- Each file name would be at max 20 characters long.
- File size can range between 0 bytes and 1GB.
