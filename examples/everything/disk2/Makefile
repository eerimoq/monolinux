create_table:
	python3 create_table.py \
	    fs.img table.img \
	    7891234871263971625789623497586239875698273465987234658792364598

unpack-fs:
	unsquashfs fs.img

pack-fs:
	rm fs.img
	mksquashfs squashfs-root fs.img # -noI -noD -noF -noX
	rm -rf squashfs-root
	$(MAKE) create_table
