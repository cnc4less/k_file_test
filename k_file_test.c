#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>




MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("test");
MODULE_VERSION("0.0");




static char* file = NULL;
module_param(file, charp, 0);

#define BUF_LEN 255
#define FNAME "/etc/hostname";
static char buff[ BUF_LEN + 1 ] = FNAME;




static int __init file_read_init( void )
{
    struct file *f;
    size_t n;
    long l;
    loff_t file_offset = 0;

    mm_segment_t fs = get_fs();
    set_fs( get_ds() );

    printk(KERN_INFO "Hi!\n");

    if ( file != NULL ) strcpy(buff, file);
    printk("(?) openning file: %s\n", buff);
    f = filp_open(buff, O_RDONLY, 0);

    if ( IS_ERR(f) )
    {
        printk("(!) file open failed: %s\n", buff);
        l = -ENOENT;
        goto fail_open;
    }

    l = vfs_llseek(f, 0L, 2); // 2 = SEEK_END
    if ( l <= 0 )
    {
        printk("(!) failed to lseek %s\n", buff);
        l = -EINVAL;
        goto fail_io;
    }
    printk("(?) file size = %d bytes\n", (int)l);

    if ( l > BUF_LEN ) l = BUF_LEN;

    vfs_llseek(f, 0L, 0); // 0 = SEEK_SET
    if ( ( n = vfs_read(f, buff, l, &file_offset) ) != l )
    {
        printk("(!) failed to read\n");
        l = -EIO;
        goto fail_io;
    }

    buff[n] = '\0';
    printk("%s\n", buff);
    printk(KERN_ALERT "(?) close file\n");
    l = 0;

fail_io:
    filp_close(f, NULL);

fail_open:
    set_fs(fs);
    return (int)l;
}




static void __exit file_read_exit(void)
{
    printk(KERN_INFO "Bye!\n");
}




module_init(file_read_init);
module_exit(file_read_exit);
