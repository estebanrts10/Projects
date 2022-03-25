#include <wb.h>

#define wbCheck(stmt)                                                     \
  do {                                                                    \
    cudaError_t err = stmt;                                               \
    if (err != cudaSuccess) {                                             \
      wbLog(ERROR, "CUDA error: ", cudaGetErrorString(err));              \
      wbLog(ERROR, "Failed to run stmt ", #stmt);                         \
      return -1;                                                          \
    }                                                                     \
  } while (0)

//@@ Define any useful program-wide constants here
#define TILE_WIDTH 4
#define MASK_WIDTH 3
//@@ Define constant memory for device kernel here
__constant__ float kernelCpy[MASK_WIDTH][MASK_WIDTH][MASK_WIDTH];

__global__ void conv3d(float *input, float *output, const int z_size,
                       const int y_size, const int x_size) {
  //@@ Insert kernel code here
  //int i = blockIdx.x * blockDim.x + threadIdx.x;
  __shared__ float tile[TILE_WIDTH+MASK_WIDTH-1][TILE_WIDTH+MASK_WIDTH-1][TILE_WIDTH+MASK_WIDTH-1];
  int radius = MASK_WIDTH / 2;
  //int start = i-radius;
  int tx = threadIdx.x;
  int ty = threadIdx.y;
  int tz = threadIdx.z;
  int Width = x_size;
  int Height = y_size;
  int Depth = z_size;
  int dep_o = blockIdx.z * TILE_WIDTH + tz;
  int row_o = blockIdx.y * TILE_WIDTH + ty;
  int col_o = blockIdx.x * TILE_WIDTH + tx;

  int dep_i = dep_o - radius;
  int row_i = row_o - radius; //mask_width / 2
  int col_i = col_o - radius; //radius

  //create width variable

  if((row_i >= 0) && (row_i < Height) && (col_i >= 0) &&
     (col_i < Width) && (dep_i >=0) && (dep_i < Depth)){
    tile[tz][ty][tx] = input[col_i + row_i*Width + dep_i*Width*Height];
    //change this input index to account for 3d

  }
  else{
    tile[tz][ty][tx] = 0.0f;
  }

  __syncthreads();
  float Pvalue = 0.0f;
  if(ty < TILE_WIDTH && tx < TILE_WIDTH && tz < TILE_WIDTH){
    for(int z = 0; z < MASK_WIDTH; z++){
      for(int y = 0; y < MASK_WIDTH; y++){
        for(int x = 0; x < MASK_WIDTH; x++){
          Pvalue += kernelCpy[z][y][x] * tile[z+tz][y+ty][x+tx];
        }
      }
    }
    if(row_o < Height && col_o < Width && dep_o < Depth){
      output[dep_o*Width*Height + row_o*Width + col_o] = Pvalue;
    }
  }

}

int main(int argc, char *argv[]) {
  wbArg_t args;
  int z_size;
  int y_size;
  int x_size;
  int inputLength, kernelLength;
  float *hostInput;
  float *hostKernel;
  float *hostOutput;
  float *deviceInput;
  float *deviceOutput;

  args = wbArg_read(argc, argv);

  // Import data
  hostInput = (float *)wbImport(wbArg_getInputFile(args, 0), &inputLength);
  hostKernel =
      (float *)wbImport(wbArg_getInputFile(args, 1), &kernelLength);
  hostOutput = (float *)malloc(inputLength * sizeof(float));

  // First three elements are the input dimensions
  z_size = hostInput[0];
  y_size = hostInput[1];
  x_size = hostInput[2];
  wbLog(TRACE, "The input size is ", z_size, "x", y_size, "x", x_size);
  assert(z_size * y_size * x_size == inputLength - 3);
  assert(kernelLength == 27);

  wbTime_start(GPU, "Doing GPU Computation (memory + compute)");

  wbTime_start(GPU, "Doing GPU memory allocation");
  //@@ Allocate GPU memory here
  cudaMalloc((void**) &deviceInput, (inputLength-3)*sizeof(float));
  cudaMalloc((void**) &deviceOutput, (inputLength-3)*sizeof(float));
  // Recall that inputLength is 3 elements longer than the input data
  // because the first  three elements were the dimensions
  wbTime_stop(GPU, "Doing GPU memory allocation");

  wbTime_start(Copy, "Copying data to the GPU");
  //@@ Copy input and kernel to GPU here

  cudaMemcpy(deviceInput, hostInput+3, (inputLength-3)*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpyToSymbol(kernelCpy, hostKernel, kernelLength*sizeof(float));
  //have to copy kernel to gpu still


  // Recall that the first three elements of hostInput are dimensions and
  // do
  // not need to be copied to the gpu
  wbTime_stop(Copy, "Copying data to the GPU");

  wbTime_start(Compute, "Doing the computation on the GPU");
  //@@ Initialize grid and block dimensions here
  dim3 dimGrid(ceil(x_size/(TILE_WIDTH*1.0)),ceil(y_size/(TILE_WIDTH*1.0)),ceil(z_size/(TILE_WIDTH*1.0)));
  dim3 dimBlock(TILE_WIDTH+MASK_WIDTH-1,TILE_WIDTH+MASK_WIDTH-1,TILE_WIDTH+MASK_WIDTH-1);

  //@@ Launch the GPU kernel here
  conv3d<<<dimGrid,dimBlock>>>(deviceInput,deviceOutput,z_size,y_size,x_size);

  cudaDeviceSynchronize();
  wbTime_stop(Compute, "Doing the computation on the GPU");

  wbTime_start(Copy, "Copying data from the GPU");
  //@@ Copy the device memory back to the host here
  cudaMemcpy(hostOutput+3, deviceOutput, (inputLength-3)*sizeof(float), cudaMemcpyDeviceToHost);
  // Recall that the first three elements of the output are the dimensions
  // and should not be set here (they are set below)
  wbTime_stop(Copy, "Copying data from the GPU");

  wbTime_stop(GPU, "Doing GPU Computation (memory + compute)");

  // Set the output dimensions for correctness checking
  hostOutput[0] = z_size;
  hostOutput[1] = y_size;
  hostOutput[2] = x_size;
  wbSolution(args, hostOutput, inputLength);

  // Free device memory
  cudaFree(deviceInput);
  cudaFree(deviceOutput);

  // Free host memory
  free(hostInput);
  free(hostOutput);
  return 0;
}
