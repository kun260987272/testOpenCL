#include<CLUtil.hpp>
#include<iostream>
using namespace std;

int main()
{


	cl_platform_id platform[2];
	cl_device_id device[2];
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel add;
	cl_int err;

	
	//1 platform
	err=clGetPlatformIDs(2, platform, NULL);

	if (err != CL_SUCCESS)
	{
		cout << "clGetPlatformIDs error" << endl;
		system("pause");
	}
	

	//显示平台名字
	for (int i = 0;i < 2;++i)
	{
		char* platformInfo;
		size_t info_size;
		// 获取大小  
		clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, 0, NULL, &info_size);
		platformInfo = new char[info_size + 1];
		// 获取信息
		clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, info_size, platformInfo, NULL);
		platformInfo[info_size] = '\0';
		cout << platformInfo << endl;
		delete[] platformInfo;
	}


	//2 device
	clGetDeviceIDs(platform[1], CL_DEVICE_TYPE_ALL, 2, device, NULL);
	if (err != CL_SUCCESS)
	{
		cout << "clGetDeviceIDs error" << endl;
		system("pause");
	}

	//显示设备名字
	for (int i = 0;i < 2;++i)
	{
		char* deviceInfo;
		size_t info_size;
		// 获取大小  
		clGetDeviceInfo(device[i], CL_DEVICE_NAME, 0, NULL, &info_size);
		deviceInfo = new char[info_size + 1];
		// 获取信息
		clGetDeviceInfo(device[i], CL_DEVICE_NAME, info_size, deviceInfo, NULL);
		deviceInfo[info_size] = '\0';
		cout << deviceInfo << endl;
		delete[] deviceInfo;
	}




	//3 context
	context = clCreateContext(NULL, 2, device, NULL, NULL,&err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateContext error" << endl;
		system("pause");
	}

	//4 queue
	queue = clCreateCommandQueue(context, device[0], 0, &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateCommandQueue error" << endl;
		system("pause");
	}

	//5 program
	FILE* fp;
	err = fopen_s(&fp, "testOpenCL.cl", "rb");
	if (err) 
	{
		cout << "fopen_s error" << endl;
		system("pause");
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] =NULL;
	fclose(fp);
	const char* source = buf;
	

	program = clCreateProgramWithSource(context, 1, &source, &size, &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateProgramWithSource error" << endl;
		system("pause");
	}

	err = clBuildProgram(program, 2, device, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateProgramWithSource error" << endl;
		system("pause");
	}

	//char* build_log;
	//size_t log_size;
	//// First call to know the proper size  
	//clGetProgramBuildInfo(program, device[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	//build_log = new char[log_size + 1];
	//// Second call to get the log  
	//clGetProgramBuildInfo(program, device[0], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
	//build_log[log_size] = '\0';
	//cout << build_log << endl;
	//delete[] build_log;

	//6 kernel
	add = clCreateKernel(program, "add", &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateKernel error" << endl;
		system("pause");
	}
	const int num=1234567;
	float* host_a=new float[num];
	float* host_b = new float[num];
	float* host_c = new float[num];
	for (int i = 0;i < num;++i)
	{
		host_a[i] = i;
		host_b[i] = i;
	}
	size_t mem_size = sizeof(float)*num;
	cl_mem dev_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, host_a, &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateBuffer a error" << endl;
		system("pause");
	}
	cl_mem dev_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, host_b, &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateBuffer b error" << endl;
		system("pause");
	}
	cl_mem dev_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY , mem_size, NULL, &err);
	if (err != CL_SUCCESS)
	{
		cout << "clCreateBuffer c error" << endl;
		system("pause");
	}

	//设置kernel参数
	err = clSetKernelArg(add, 0, sizeof(cl_mem), &dev_a);
	err |= clSetKernelArg(add, 1, sizeof(cl_mem), &dev_b);
	err |= clSetKernelArg(add, 2, sizeof(cl_mem), &dev_c);
	if (err != CL_SUCCESS)
	{
		cout << "clSetKernelArg error" << endl;
		system("pause");
	}

	//调用kernel
	const size_t global_work_size = num;
	const size_t local_work_size = 64;

	int start = clock();

	err = clEnqueueNDRangeKernel(queue, add, 1, 0, &global_work_size, 0, NULL, NULL, NULL);
	//err = clEnqueueNDRangeKernel(queue, add, 1, 0, &global_work_size, &local_work_size, NULL, NULL, NULL);
	
	int end = clock();
	cout << end - start << endl;
	if (err != CL_SUCCESS)
	{
		cout << "clEnqueueNDRangeKernel error" << endl;
		system("pause");
	}
	//7 result
	err = clEnqueueReadBuffer(queue, dev_c, CL_TRUE, 0, mem_size, host_c, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		cout << "clEnqueueReadBuffer error" << endl;
		system("pause");
	}
	
	

	for (int i = 0;i < 100;++i)
	{
		cout << host_c[i] << "  ";
	}

	//8 clean
	delete[] host_a;
	delete[] host_b;
	delete[] host_c;
	clReleaseMemObject(dev_a);
	clReleaseMemObject(dev_b);
	clReleaseMemObject(dev_c);
	clReleaseKernel(add);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	
	



	system("pause");
	return 0;
}



