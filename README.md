# Mini-Operating-System

Library components included:
	make_fs creates the file and loads metadata
	mount_fs loads metadata from disk into memory
	unmount_fs writes metadata back to disk
	fs_open opens a file and returns its location in the directory list
	fs_close does nothing since file descriptors have been omitted
	fs_create creates a directory entry (encompasses mkdir)
	fs_delete deletes the directory entry and frees its space
	fs_read reads the data from the file
	fs_write writes data to the file
	
The test program is menu driven and contains a user-controlled terminal I/O loop to test each function.
