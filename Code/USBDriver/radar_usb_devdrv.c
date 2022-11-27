#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie Sands");
MODULE_DESCRIPTION("Linux driver for ckzor'  2.4ish GHz Radar");

#define USB_VID 0x1209
#define USB_PID 0x5955

/* ProcFS Codes */

static struct proc_dir_entry *control_proc_file;
static struct proc_dir_entry *bulk_proc_file;
static struct usb_device *usb_dev;

static char bulk_transfer_buffer[1024];

static ssize_t radar_control_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	char text[32];
	int to_copy, not_copied, delta, status;
	u8 val;

	to_copy = min(count, sizeof(text));

	status = usb_control_msg_recv(usb_dev,
			usb_rcvctrlpipe(usb_dev, 0x03),
			0x2,
			0xC0,
			0,
			0,
			(unsigned char *) &val,
			1,
			100,
			GFP_KERNEL);

	if(status < 0) {
		printk("radar_usb_devdrv error during usb recieve");
		return -1;
	}
	sprintf(text, "0x%x\n", val);

	not_copied = copy_to_user(user_buffer, text, to_copy);

	delta = to_copy - not_copied;

	return delta;
}

static ssize_t radar_control_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	char text[255];
	int to_copy, not_copied, delta, status;
	long val;
	u16 seg_val;

	memset(text, 0, sizeof(text));

	to_copy = min(count, sizeof(text));

	not_copied = copy_from_user(text, user_buffer, to_copy);
	printk("radar_usb_devdrv written to procfs %s\n", text);
	if(0 != kstrtol(text, 0, &val)) {
		printk("radar_usb_devdrv error converting procfs input");
		return -1;
	}

	seg_val = (u16) val;
	status = usb_control_msg_send(usb_dev, usb_sndctrlpipe(usb_dev, 0x03), 0x1, 0x40, seg_val, 0, NULL, 0, 100, GFP_KERNEL);
	if(status < 0) {
		printk("radar_usb_devdrv error during usb send");
		return -1;
	}

	delta = to_copy - not_copied;

	return delta;
}

static void radar_bulk_recv_callback(struct urb *recv_urb) {
	printk("%s\n", (char*) recv_urb->transfer_buffer);
	return;
}

static ssize_t radar_bulk_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	const char* string = "Not Implemented Yet\n";
	struct urb* fill_urb;
	int status;

	printk("radar_usb_devdrv radar_bulk_read 1");
	memset(bulk_transfer_buffer, 0, sizeof(bulk_transfer_buffer));

	printk("radar_usb_devdrv radar_bulk_read 2");
	fill_urb = usb_alloc_urb(0, GFP_KERNEL);

	printk("radar_usb_devdrv radar_bulk_read 3");
	usb_fill_bulk_urb(fill_urb, 
			usb_dev,
			usb_sndctrlpipe(usb_dev, 0x03),
			bulk_transfer_buffer,
			sizeof(bulk_transfer_buffer),
			(usb_complete_t) radar_bulk_recv_callback,
			NULL);

	printk("radar_usb_devdrv radar_bulk_read 4");
	status = usb_submit_urb(fill_urb, GFP_KERNEL);

	if(status < 0) {
		printk("radar_usb_devdrv error during usb bulk recieve");
		return -1;
	}

	printk("radar_usb_devdrv radar_bulk_read 5");
	status = copy_to_user(user_buffer, string, sizeof(string));

	return 0;
}

static struct proc_ops control_fops = {
	.proc_read = radar_control_read,
	.proc_write = radar_control_write,
};

static struct proc_ops bulk_fops = {
	.proc_read = radar_bulk_read,
	.proc_write = NULL,
};

/* USB Codes */

static struct usb_device_id radar_usb_table[] = {
	{ USB_DEVICE(USB_VID, USB_PID) },
	{},
};


MODULE_DEVICE_TABLE(usb, radar_usb_table);

static int radar_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	printk("radar_usb_devdrv probe\n");

	usb_dev = interface_to_usbdev(intf);
	if(usb_dev == NULL) {
		printk("radar_usb_devdrv error in getting usb_dev from interface\n");
		return -ENOMEM;
	}

	control_proc_file = proc_create("radar_usb_dev_control", 0666, NULL, &control_fops);
	if(control_proc_file == NULL) {
		printk("radar_usb_devdrv error in creating control procfs file\n");
		return -ENOMEM;
	}
	
	bulk_proc_file = proc_create("radar_usb_dev_bulk", 0444, NULL, &bulk_fops);
	if(bulk_proc_file == NULL) {
		printk("radar_usb_devdrv error in creating bulk procfs file\n");
		return -ENOMEM;
	}

	return 0;
}

static void radar_usb_disconnect(struct usb_interface *intf) {
	proc_remove(control_proc_file);
	proc_remove(bulk_proc_file);
	printk("radar_usb_devdrv disconnect\n");
}

static struct usb_driver radar_usb_driver = {
	.name = "radar_usb_devdrv",
	.id_table = radar_usb_table,
	.probe = radar_usb_probe,
	.disconnect = radar_usb_disconnect,
};

/* Main Codes */

static int __init radar_init(void) {
	int result;
	printk("radar_usb_devdrv load\n");

	result = usb_register(&radar_usb_driver);
	if(result) {
		printk("radar_usb_devdrv usb_register error");
		return -1;
	}

	return 0;
}

static void __exit radar_exit(void) {
	printk("radar_usb_devdrv exit\n");
	
	usb_deregister(&radar_usb_driver);
}

module_init(radar_init);
module_exit(radar_exit);
