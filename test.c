#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <asm/uaccess.h>

int a=4, b=13;
char c='*';

int result_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	char output[16];

	if (*ppos != 0)
		return 0;

	switch (c) {
	case '+':
		sprintf(output, "%d + %d = %d\n", a, b, a+b);
		break;
	case '-':
                sprintf(output, "%d - %d = %d\n", a, b, a-b);
                break;
	case '*':
                sprintf(output, "%d * %d = %d\n", a, b, a*b);
                break;
	case '/':
                sprintf(output, "%d / %d = %d\n", a, b, a/b);
                break;
	default:
		break;
	}

	strcpy(buffer, output);
	*ppos = strlen(output);
	return strlen(output);
}

int write_operand1(struct file *file, const char *buffer, unsigned long count, void *data)
{
        char input[16];
        printk(KERN_INFO "write_operand1 is called, buffer[0]=%c.\n", buffer[0]);
        if (copy_from_user(input, buffer, count))
                return -EFAULT;
        sscanf(input, "%d", &a);
        return count;
}

int write_operand2(struct file *file, const char *buffer, unsigned long count, void *data)
{
        char input[16];
        printk(KERN_INFO "write_operand2 is called.\n");
        if (copy_from_user(input, buffer, count))
                return -EFAULT;
        sscanf(input, "%d", &b);
        return count;
}

int write_operator(struct file *file, const char *buffer, unsigned long count, void *data)
{
        char input[16];
        printk(KERN_INFO "write_operator is called.\n");
        if (copy_from_user(input, buffer, count))
                return -EFAULT;
        c = input[0];
        return count;
}

/////////////////
static const struct file_operations result_fops = {
        .owner = THIS_MODULE,
        .read = result_read,
};

static struct miscdevice result_dev = {
	MISC_DYNAMIC_MINOR, "test_result", &result_fops
};

/////////////////
static const struct file_operations operand1_fops = {
        .owner = THIS_MODULE,
        .write = write_operand1,
};

static struct miscdevice operand1_dev = {
        MISC_DYNAMIC_MINOR, "test_operand1", &operand1_fops
};
////
static const struct file_operations operand2_fops = {
        .owner = THIS_MODULE,
        .write = write_operand2,
};

static struct miscdevice operand2_dev = {
        MISC_DYNAMIC_MINOR, "test_operand2", &operand2_fops
};
////
static const struct file_operations operator_fops = {
        .owner = THIS_MODULE,
        .write = write_operator,
};

static struct miscdevice operator_dev = {
        MISC_DYNAMIC_MINOR, "test_operator", &operator_fops
};
/////////////////

int __init test_init(void)
{
	printk(KERN_INFO "test module is loaded\n");
	if (misc_register(&result_dev))
		printk(KERN_ERR "Unable to register test misc devise!\n");
	if (misc_register(&operand1_dev))
                printk(KERN_ERR "Unable to register test misc devise!\n");
	if (misc_register(&operand2_dev))
                printk(KERN_ERR "Unable to register test misc devise!\n");
	if (misc_register(&operator_dev))
                printk(KERN_ERR "Unable to register test misc devise!\n");
	return 0;
}

module_init(test_init);

static void __exit test_exit(void)
{
	misc_deregister(&result_dev);
	misc_deregister(&operand1_dev);
	misc_deregister(&operand2_dev);
	misc_deregister(&operator_dev);
}

module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Pletnev <pletnevroman95@gmail.com>");
MODULE_DESCRIPTION("\"Hello, world!\" minimal module");
