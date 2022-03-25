// MP Scan
// Given a list (lst) of length n
// Output its prefix sum = {lst[0], lst[0] + lst[1], lst[0] + lst[1] + ...
// +
// lst[n-1]}

#include <wb.h>

#define BLOCK_SIZE 512 //@@ You can change this

#define wbCheck(stmt)                                                     \
  do {                                                                    \
    cudaError_t err = stmt;                                               \
    if (err != cudaSuccess) {                                             \
      wbLog(ERROR, "Failed to run stmt ", #stmt);                         \
      wbLog(ERROR, "Got CUDA error ...  ", cudaGetErrorString(err));      \
      return -1;                                                          \
    }                                                                     \
  } while (0)
//__global__ int flag = 0;
__global__ void scan(float *input, float *output, int len) {
  __shared__ float T[2*BLOCK_SIZE];
  //int indexTemp;
  /*int temp = 1;
  int strideTemp = blockDim.x + blockDim.x*flag;
  if(flag == 0){
    temp = blockIdx.x;
  }
  int indexTemp = threadIdx.x+flag + temp * blockDim.x*2 - flag;
  */
  int i = 2*blockIdx.x*blockDim.x + threadIdx.x;
  if(i<len)
    T[threadIdx.x] = input[i];
  if (i+blockDim.x < len)
    T[threadIdx.x+blockDim.x]=input[i+blockDim.x];

  /*int stride = 1;
  while(stride<2*BLOCK_SIZE){
    __syncthreads();
    int index = (threadIdx.x+1)*stride*2 - 1;
    if(index < 2*BLOCK_SIZE && (index-stride) >=0){
      T[index] += T[index-stride];
    }
    stride *= 2;
  }



  stride = BLOCK_SIZE/2;
  while(stride > 0){
    __syncthreads();
    int index = (threadIdx.x+1)*stride*2 - 1;
    if((index+stride) < 2*BLOCK_SIZE){
      T[index+stride] += T[index];
    }
    stride = stride/2;
  }*/
  for (unsigned int stride = 1; stride <= blockDim.x;stride *= 2){
    __syncthreads();
    int index = (threadIdx.x+1)* 2* stride -1;
    if(index<2*BLOCK_SIZE)
      T[index] += T[index-stride];
  }
  for (int stride = BLOCK_SIZE/2;stride>0; stride /= 2){
    __syncthreads();
    int index = (threadIdx.x+1)*stride*2 - 1;
    if(index + stride < BLOCK_SIZE*2)
      T[index + stride] += T[index];
  }

  __syncthreads();
  if(i<len)
    output[i] = T[threadIdx.x];
  if(i+blockDim.x<len)
    output[i+blockDim.x] = T[threadIdx.x+blockDim.x];






  //@@ Modify the body of this function to complete the functionality of
  //@@ the scan on the device
  //@@ You may need multiple kernel calls; write your kernels before this
  //@@ function and call them from the host
  /*if(flag == 0)
    flag = 1;
  else
    flag = 0;
*/
}

int main(int argc, char **argv) {
  wbArg_t args;
  float *hostInput;  // The input 1D list
  float *hostOutput; // The output list
  float *deviceInput;
  float *deviceOutput;
  int numElements; // number of elements in the list

  args = wbArg_read(argc, argv);

  wbTime_start(Generic, "Importing data and creating memory on host");
  hostInput = (float *)wbImport(wbArg_getInputFile(args, 0), &numElements);
  hostOutput = (float *)malloc(numElements * sizeof(float));
  wbTime_stop(Generic, "Importing data and creating memory on host");

  wbLog(TRACE, "The number of input elements in the input is ",
        numElements);

  wbTime_start(GPU, "Allocating GPU memory.");
  wbCheck(cudaMalloc((void **)&deviceInput, numElements * sizeof(float)));
  wbCheck(cudaMalloc((void **)&deviceOutput, numElements * sizeof(float)));
  wbTime_stop(GPU, "Allocating GPU memory.");

  wbTime_start(GPU, "Clearing output memory.");
  wbCheck(cudaMemset(deviceOutput, 0, numElements * sizeof(float)));
  wbTime_stop(GPU, "Clearing output memory.");

  wbTime_start(GPU, "Copying input memory to the GPU.");
  wbCheck(cudaMemcpy(deviceInput, hostInput, numElements * sizeof(float),
                     cudaMemcpyHostToDevice));
  wbTime_stop(GPU, "Copying input memory to the GPU.");

  //@@ Initialize the grid and block dimensions here
  dim3 dimGrid(numElements,1,1);
  dim3 dimBlock(BLOCK_SIZE,1,1);
  scan<<<dimGrid,dimBlock>>>(deviceInput, deviceOutput, numElements);

  wbTime_start(Compute, "Performing CUDA computation");
  //@@ Modify this to complete the functionality of the scan
  //@@ on the deivce

  cudaDeviceSynchronize();
  wbTime_stop(Compute, "Performing CUDA computation");

  wbTime_start(Copy, "Copying output memory to the CPU");
  wbCheck(cudaMemcpy(hostOutput, deviceOutput, numElements * sizeof(float),
                     cudaMemcpyDeviceToHost));
  wbTime_stop(Copy, "Copying output memory to the CPU");

  wbTime_start(GPU, "Freeing GPU Memory");
  cudaFree(deviceInput);
  cudaFree(deviceOutput);
  wbTime_stop(GPU, "Freeing GPU Memory");

  wbSolution(args, hostOutput, numElements);

  free(hostInput);
  free(hostOutput);

  return 0;
}
