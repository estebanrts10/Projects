// Histogram Equalization

#include <wb.h>
#define HISTOGRAM_LENGTH 256

__global__ void floatToChar(float* floot, unsigned char* chor, int width, int height, int imageChannels){
  int i = threadIdx.x + blockIdx.x*blockDim.x;
  if(i < width*height*imageChannels){
    chor[i] = (unsigned char)(255*floot[i]);
  }
}
//change for loops to if statements

__global__ void RGBtoGray(unsigned char* ucharImage, unsigned char* grayImage, int width,int height, int imageChannels){
  int x = threadIdx.x + blockIdx.x*blockDim.x;
  int y = threadIdx.y + blockIdx.y*blockDim.y;
  if(threadIdx.x + blockIdx.x*blockDim.x < width*height*imageChannels){
    int idx = y*width+x;
    float r = ucharImage[3*idx];
    float g = ucharImage[3*idx+1];
    float b = ucharImage[3*idx+2];
    grayImage[idx] = (unsigned char)(0.21*r + 0.71*g + 0.07*b);

  }
}


//@@ insert code here
__global__ void histo_kernel(unsigned char *buffer, int size, unsigned int *histo){
  __shared__ unsigned int private_histo[256];
  if(threadIdx.x < 256) private_histo[threadIdx.x] = 0;
  __syncthreads();
  int i = threadIdx.x + blockIdx.x*blockDim.x;
  int stride = blockDim.x * gridDim.x;
  while(i<size){
    atomicAdd(&(private_histo[buffer[i]]),1);
    i += stride;
  }
  __syncthreads();
  if(threadIdx.x < 256){
    atomicAdd(&(histo[threadIdx.x]), private_histo[threadIdx.x]);
  }
}

__global__ void scan(unsigned int *input, float *output, int len, int width, int height) {
  __shared__ float T[HISTOGRAM_LENGTH];
  int i = 2*blockIdx.x*blockDim.x + threadIdx.x;
  if(i<len)
    T[threadIdx.x] = input[i];
  if (i+blockDim.x < len)
    T[threadIdx.x+blockDim.x]=input[i+blockDim.x];

  for (unsigned int stride = 1; stride <= blockDim.x;stride *= 2){
    __syncthreads();
    int index = (threadIdx.x+1)* 2* stride -1;
    if(index<HISTOGRAM_LENGTH)
      T[index] += T[index-stride];
  }
  for (int stride = 64;stride>0; stride /= 2){
    __syncthreads();
    int index = (threadIdx.x+1)*stride*2 - 1;
    if(index + stride < HISTOGRAM_LENGTH)
      T[index + stride] += T[index];
  }

  __syncthreads();
  if(i<len)
    output[i] = T[threadIdx.x]/(1.0*width*height);
  if(i+blockDim.x<len)
    output[i+blockDim.x] = T[threadIdx.x+blockDim.x]/(1.0*width*height);

}



__global__ void histEqual(float* cdf, unsigned char* out, int width, int height, int imageChannels){
  //cdf = in
  int idx = threadIdx.x + blockIdx.x*blockDim.x;
  //float val = out[idx];
  //float cdfmin = cdf[0];
  if(idx < width*height*imageChannels){
    out[idx] = (unsigned char) (min(max(255.0*(cdf[out[idx]]-cdf[0])/(1.0-cdf[0]),0.0),255.0));
  }
}


__global__ void charToFloat(unsigned char* in, float* out, int width, int height, int imageChannels){
  int i = threadIdx.x + blockIdx.x*blockDim.x;

  if(i<width*height*imageChannels){
    out[i] = (float) (in[i]/255.0);
  }
}




int main(int argc, char **argv) {
  wbArg_t args;
  int imageWidth;
  int imageHeight;
  int imageChannels;
  int imageWH;
  wbImage_t inputImage;
  wbImage_t outputImage;
  float *hostInputImageData;
  float *hostOutputImageData;
  const char *inputImageFile;

  //@@ Insert more code here
  unsigned char* uchar;
  float* floot;
  unsigned int* kurnel;
  unsigned char* gray;
  float* scan1;

  args = wbArg_read(argc, argv); /* parse the input arguments */

  inputImageFile = wbArg_getInputFile(args, 0);

  wbTime_start(Generic, "Importing data and creating memory on host");
  inputImage = wbImport(inputImageFile);
  imageWidth = wbImage_getWidth(inputImage);
  imageHeight = wbImage_getHeight(inputImage);
  imageChannels = wbImage_getChannels(inputImage);
  outputImage = wbImage_new(imageWidth, imageHeight, imageChannels);
  hostInputImageData = wbImage_getData(inputImage);
  hostOutputImageData = wbImage_getData(outputImage);
  wbTime_stop(Generic, "Importing data and creating memory on host");
  imageWH = imageWidth*imageHeight;
  //@@ insert code here
  cudaMalloc((void**)&uchar, imageWH*imageChannels*sizeof(unsigned char));
  cudaMalloc((void**)&floot, imageWH*imageChannels*sizeof(float));
  cudaMalloc((void**)&kurnel, HISTOGRAM_LENGTH*sizeof(unsigned int));
  cudaMalloc((void**)&gray, imageWH*sizeof(unsigned char));
  cudaMalloc((void**)&scan1, HISTOGRAM_LENGTH*sizeof(float));

  cudaMemcpy(floot, hostInputImageData, imageWH*imageChannels*sizeof(float),cudaMemcpyHostToDevice);
  int twoLen = HISTOGRAM_LENGTH*2;
  //1
  dim3 dimGridChar(ceil(imageWH*imageChannels*1.0/twoLen),1,1);
  dim3 dimBlockChar(twoLen,1,1);
  floatToChar<<<dimGridChar,dimBlockChar>>>(floot, uchar, imageWidth, imageHeight, imageChannels);
  cudaDeviceSynchronize();
  //2
  dim3 dimGridGray(ceil(imageWH*1.0/twoLen),1,1);
  dim3 dimBlockGray(twoLen,1,1);
  RGBtoGray<<<dimGridGray,dimBlockGray>>>(uchar, gray, imageWidth, imageHeight, imageChannels);
  cudaDeviceSynchronize();
  //3
  dim3 dimGridKurnel(ceil(imageWH*1.0/HISTOGRAM_LENGTH),1,1);
  dim3 dimBlockKurnel(HISTOGRAM_LENGTH,1,1);
  histo_kernel<<<dimGridKurnel,dimBlockKurnel>>>(gray,imageWH,kurnel);
  cudaDeviceSynchronize();
  //4
  dim3 dimGridScan(1,1,1);
  dim3 dimBlockScan(128,1,1);
  scan<<<dimGridScan,dimBlockScan>>>(kurnel,scan1,HISTOGRAM_LENGTH,imageWidth,imageHeight);
  cudaDeviceSynchronize();
  //5
  dim3 dimGridEqual(ceil(imageWH*imageChannels*1.0/twoLen),1,1);
  dim3 dimBlockEqual(twoLen,1,1);
  histEqual<<<dimGridEqual, dimBlockEqual>>>(scan1,uchar,imageWidth,imageHeight,imageChannels);
  cudaDeviceSynchronize();
  //6
  dim3 dimGrid(ceil(imageWH*imageChannels*1.0/twoLen),1,1);
  dim3 dimBlock(twoLen,1,1);
  charToFloat<<<dimGrid,dimBlock>>>(uchar, floot, imageWidth, imageHeight, imageChannels);
  cudaDeviceSynchronize();

  cudaMemcpy(hostOutputImageData, floot,imageWH*imageChannels*sizeof(float),cudaMemcpyDeviceToHost);
  wbImage_setData(outputImage,hostOutputImageData);
  wbSolution(args, outputImage);
  cudaFree(hostInputImageData);
  cudaFree(hostOutputImageData);
  cudaFree(uchar);
  cudaFree(floot);
  cudaFree(kurnel);
  cudaFree(gray);
  cudaFree(scan1);

  //@@ insert code here


  return 0;
}
