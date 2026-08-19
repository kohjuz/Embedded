2-2:
	aarch64-linux-gnu-gcc 2-2.c jpg.c bmp.c png.c -o 2-2 \
		-I /home/gec/io/0818 \
		-L /home/gec/io/0818 -lLCD \
		-I /home/gec/0815/jpeg-9d/build/include/ \
		-L /home/gec/0815/jpeg-9d/build/lib/ -ljpeg
	scp 2-2 root@192.168.55.11:/root/0819

clean:
	rm -rf 2-2