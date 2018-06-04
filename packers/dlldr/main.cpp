int keystone_patch(int argc, char *argv[]);

unsigned long __stdcall alignment(unsigned long region_size, unsigned long image_aligin)
{
	unsigned long mod = region_size % image_aligin;
	region_size -= mod;

	return region_size + image_aligin;
}

bool WriteGetModuleHandle();
int main(int argc, char *argv[])
{
	keystone_patch(argc, argv);
}

