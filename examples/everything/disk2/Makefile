create_table:
	python3 create_table.py

unpack-fs:
	unsquashfs fs.img

pack-fs:
	rm fs.img
	mksquashfs squashfs-root fs.img # -noI -noD -noF -noX
	rm -rf squashfs-root
	$(MAKE) create_table
