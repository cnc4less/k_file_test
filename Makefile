PNAME = k_file_test

obj-m += $(PNAME).o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	sudo dmesg -C
	sudo insmod $(PNAME).ko file=/etc/hosts
	sudo rmmod $(PNAME).ko
	dmesg
