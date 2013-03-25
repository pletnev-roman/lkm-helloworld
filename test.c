#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/skbuff.h>

static struct proc_dir_entry *mf_proc_file_operand1;
static struct proc_dir_entry *mf_proc_file_operand2;
static struct proc_dir_entry *mf_proc_file_operator;
int a, b;
char c=']';
bool gota=0, gotb=0;

int read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data)
{
	char output[16];

	if (offset != 0)
		return 0;

	if ((!gota) || (!gotb) || ((c != '+')&&(c != '-')&&(c != '*')&&(c != '/'))) {
		char err_output[] = "Some of parameters are not initialized!\n";
		strcpy(buffer, err_output);
		*eof = 1;
		return strlen(err_output);
	}

	switch (c) {
	case '+':
		sprintf(output, "%d\n", a+b);
		break;
	case '-':
                sprintf(output, "%d\n", a-b);
                break;
	case '*':
                sprintf(output, "%d\n", a*b);
                break;
	case '/':
                sprintf(output, "%d\n", a/b);
                break;
	default:
		break;
	}

	strcpy(buffer, output);
	*eof = 1;
	return strlen(output);
}

int procf_write_operand1(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char input[16];
	printk(KERN_INFO "procf_write_operand1 is called.\n");
	if (copy_from_user(input, buffer, count))
		return -EFAULT;
	sscanf(input, "%d", &a);
	gota = 1;
	return count;
}

int procf_write_operand2(struct file *file, const char *buffer, unsigned long count, void *data)
{
        char input[16];
	printk(KERN_INFO "procf_write_operand2 is called.\n");
        if (copy_from_user(input, buffer, count))
                return -EFAULT;
        sscanf(input, "%d", &b);
	gotb = 1;
        return count;
}

int procf_write_operator(struct file *file, const char *buffer, unsigned long count, void *data)
{
        char input[16];
	printk(KERN_INFO "procf_write_operator is called.\n");
        if (copy_from_user(input, buffer, count))
                return -EFAULT;
        c = input[0];
        return count;
}

int __init test_init(void)
{
	printk(KERN_INFO "test module is loaded\n");

	mf_proc_file_operand1 = create_proc_entry("test_operand1.in", 0644, NULL);
	mf_proc_file_operand2 = create_proc_entry("test_operand2.in", 0644, NULL);
	mf_proc_file_operator = create_proc_entry("test_operator.in", 0644, NULL);

	if (mf_proc_file_operand1 == NULL) {
        	printk(KERN_INFO "Error: could not initialize /proc/test_operand1.in\n");
        	return -ENOMEM;
	}
	if (mf_proc_file_operand2 == NULL) {
                printk(KERN_INFO "Error: could not initialize /proc/test_operand2.in\n");
                return -ENOMEM;
        }
	if (mf_proc_file_operator == NULL) {
                printk(KERN_INFO "Error: could not initialize /proc/test_operator.in\n");
                return -ENOMEM;
        }

	mf_proc_file_operand1->write_proc = procf_write_operand1;
	mf_proc_file_operand2->write_proc = procf_write_operand2;
	mf_proc_file_operator->write_proc = procf_write_operator;

	if (create_proc_read_entry("test.out", 0, NULL, read_proc, NULL) == 0) {
		printk(KERN_ERR "Unable to register \"test.out\" proc file\n");
		return -ENOMEM;
	}

	return 0;
}

module_init(test_init);

static void __exit test_exit(void)
{
	remove_proc_entry("test.out", NULL);
	remove_proc_entry("test_operand1.in", NULL);
	remove_proc_entry("test_operand2.in", NULL);
	remove_proc_entry("test_operator.in", NULL);
	printk(KERN_INFO "test module is unloaded");
}

module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Pletnev <pletnevroman95@gmail.com>");
MODULE_DESCRIPTION("\"Hello, world!\" minimal module");
