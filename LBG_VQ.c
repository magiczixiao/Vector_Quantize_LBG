/*********************
 * 使用LBG算法压缩lena512.raw,生成codebook.raw
 * 参数设定:
 * 图像大小:512*512
 * 码书大小:32
 * 码字大小:8*8
 * 编译环境:Linux gcc
 * 请输入指令: source compile.sh 进行编译
 * 
 * 编码输入文件:lena512x512.raw
 * 编码输出文件:codebook.raw
 * 
 * 王子潇 18120320
 * 2018.11.23
 * *********************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#define IMAGE_SIZE_X 512    //设定待压缩灰度图像大小
#define IMAGE_SIZE_Y 512
#define CODE_BOOK_SIZE 32   //码书大小
#define CODE_VECTOR_X 8     //码字X方向大小
#define CODE_VECTOR_Y 8     //码字Y方向大小
#define CODE_VECTOR_SIZE 64 //CODE_VECTOR_X * CODE_VECTOR_Y
#define ERROR_RATE 0.001    //LBG迭代损失阈值
#define INPUT_FILE_NAME "lena512x512.raw"
#define OUTPUT_FILE_NAME "codebook.raw"

#define __DEBUG//输出调试信息

typedef unsigned char code_t;
typedef unsigned char index_t;
int main()
{
    printf("Begin compressing...\n");
    int i,j,Size_Origin;
    int rows, cols, parts;
    int width, height;
    FILE *fp_in, *fp_out;
    code_t codebook[CODE_BOOK_SIZE][CODE_VECTOR_SIZE];
    //读取原数据
    fp_in = fopen(INPUT_FILE_NAME,"rb");
    fp_out = fopen(OUTPUT_FILE_NAME,"wb");
    if(!fp_in || !fp_out){
        printf("Error while opening lena512x512.raw!\nProgrom terminated\n");
        return -1;
    }else{
        fseek(fp_in, 0, SEEK_END);
        Size_Origin = ftell(fp_in);
        fseek(fp_in, 0, SEEK_SET);
    }
    if(Size_Origin % CODE_VECTOR_SIZE != 0){
        printf("error\n");
        return -1;
    }
    width = Size_Origin/IMAGE_SIZE_X;
    height = Size_Origin/IMAGE_SIZE_Y;
    
    #ifdef __DEBUG
    printf("width = %d\n",width);
    printf("height = %d\n",height);
    #endif

    rows =  width/CODE_VECTOR_X;//用于记录分成块的行/列
    cols =  width/CODE_VECTOR_Y;
    parts = Size_Origin/CODE_VECTOR_SIZE;//总共分成64*64个8*8的块

    #ifdef __DEBUG 
    printf("rows = cols = %d\n",rows);
    printf("Size_Origin = %d\n",Size_Origin);
    printf("parts = %d\n",parts);
    #endif

    code_t *data=(code_t *)malloc(Size_Origin * sizeof(code_t));//rows*cols
    fread(data,sizeof(code_t),Size_Origin,fp_in);
    //用 CODE_VECTOR_SIZE 将512*512的图像分块->64*64块
    code_t *data_block=(code_t *)malloc(Size_Origin * sizeof(code_t));
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            int tmp_i = i/CODE_VECTOR_X * cols + j/CODE_VECTOR_X;
            int tmp_j = i%CODE_VECTOR_X*CODE_VECTOR_X + j%CODE_VECTOR_X;
            data_block[tmp_i*CODE_VECTOR_SIZE + tmp_j] = data[i*width + j];
            if(i==0 && j==0){
            }
        }
    }

    #ifdef __DEBUG
    printf("Cut the data DONE!\n");
    #endif

    //初始化码书,随机从64*64个块中选取32个
    srand((unsigned int) time(NULL)); //产生随机数

    for(i = 0; i < CODE_BOOK_SIZE; i++)
    {
        for(j = 0; j < CODE_VECTOR_SIZE; j++)
        {
            int rand_i = rand() % parts;
            //printf("rand_i = %d\n",rand_i);
            codebook[i][j] = data_block[rand_i*CODE_VECTOR_SIZE+j];//用分块后的数据随机初始化码书
            //printf("codebook[%d][%d] = %d\n", i, j, codebook[i][j]);
        }
    }

    #ifdef __DEBUG
    printf("Init codebook DONE!\n");
    #endif


    //LBG algorithm

    #ifdef __DEBUG
    printf("Running LBG algorithm...\n");
    #endif

    double d0 = 0.0;
    double d1 = 0.0;
    int near_count[CODE_BOOK_SIZE];
    index_t near_code[parts];
    for(i = 0; i < parts; i++)
    {
        for(j = 0; j < CODE_VECTOR_SIZE; j++)
        {
            d0 += pow(data_block[i*CODE_VECTOR_SIZE+j]-codebook[0][j], 2);//计算每个像素点到码书中点的距离,求和
        }
    }

    #ifdef __DEBUG
    printf("Compute d0 DONE!\n");
    #endif

    int algo_count = 0;

    while(1)//当 (d0-d1)/d1 < ERROR_RATE完成训练,break退出循环
    {
        algo_count++;
        d1 = 0.0;
        for(i = 0; i < CODE_BOOK_SIZE; i++){
            near_count[i] = 0;
        }
        for(i = 0; i < parts; i++)
        {
            double min = 0.0;
            near_code[i] = 0;
            for(j = 0; j < CODE_VECTOR_SIZE; j++)
            {//min 第一次与d0取值相同
                min += pow(data_block[i*CODE_VECTOR_SIZE+j] - codebook[0][j], 2);
            }
            for(j = 1; j < CODE_BOOK_SIZE; j++)
            {
                double d = 0.0;
                for(int k = 0; k < CODE_VECTOR_SIZE; k++)
                {
                    d += pow(data_block[i*CODE_VECTOR_SIZE+k]-codebook[j][k], 2);
                    if(d > min)
                    {
                        break;
                    }
                }
                if(d < min)//min记录最小距离
                {
                    min = d;
                    near_code[i] = j;
                }
            }
            near_count[near_code[i]] += 1;
            d1 += min;
        }
        #ifdef __DEBUG
        printf("\rRunning epoch:%d, (d0-d1)/d = %f\tERROR_RATE = %.3f\n",algo_count, (d0 - d1)/d1, ERROR_RATE);
        #endif
        fflush(stdout);//强制将缓冲区内的数据写回stdout文件中

        if(abs((d0 - d1))/d1 < ERROR_RATE)
        {
            break;
        }
        d0 = d1;
        for(int i = 0; i < CODE_BOOK_SIZE; i++)
        {
            if(near_count[i] != 0)
            {
                int tmp_part[CODE_VECTOR_SIZE] = {0};
                for(j = 0; j < parts; j++)
                {
                    if(near_code[j] == i)
                    {
                        for(int k = 0; k < CODE_VECTOR_SIZE; k++)
                        {
                            tmp_part[k] += data_block[j*CODE_VECTOR_SIZE+k];
                        }
                    }
                }
                for(j = 0; j < CODE_VECTOR_SIZE; j++)
                {
                    codebook[i][j] = tmp_part[j] / near_count[i];
                }
            }
            else
            {
                for(j = 0; j < CODE_VECTOR_SIZE; j++)
                {
                    int rand_i = rand() % parts;
                    codebook[i][j] = data_block[rand_i*CODE_VECTOR_SIZE+j];
                }
            }
        }
    }

    //保存码书
    int write_buffer =  CODE_BOOK_SIZE;
    fwrite(&write_buffer, sizeof(int), 1, fp_out);//第一字节:码书大小(32个->20H)第一字节:20 00 00 0
    write_buffer = CODE_VECTOR_X;
    fwrite(&write_buffer, sizeof(int), 1, fp_out);//第二字节:CODE_VECTOR_X(8bit->08H)第二字节:08 00 00 00
    write_buffer =  width;
    fwrite(&write_buffer, sizeof(int), 1, fp_out);//第三字节:图像宽度(512->200H)第三字节:00 02 00 0

    #ifdef __DEBUG
    printf("CODE_BOOK_SIZE = %d\n",CODE_BOOK_SIZE);
    printf("CODE_VECTOR_X = %d\n",CODE_VECTOR_X);
    #endif

    for(i = 0; i < CODE_BOOK_SIZE; i++)//第4->4+32*8*8=2051(803H)字节:存储码书
    {
        for(j = 0; j < CODE_VECTOR_SIZE; j++)
        {
            fwrite(&codebook[i][j], sizeof(code_t), 1, fp_out);
        }
    }
    printf("Code book in generated successfully! Saved in 'codebook.raw'\n");

    //保存索引
    for(i = 0; i < parts; i++)//第2052->(2052+4096=6148)字节:存储索引
    {
        fwrite(&near_code[i], sizeof(index_t), 1, fp_out);
    }

    free(data);
    free(data_block);
    fseek(fp_out, 0, SEEK_END);
    int Size_Compressed = ftell(fp_out);//压缩后的文件大小应该为6148字节
    fclose(fp_in);
    fclose(fp_out); 
    float ratio = (float)Size_Compressed/Size_Origin*100;
    
    #ifdef __DEBUG
    printf("Compression complete!\n");
    printf("\nImage size: %d*%d, CODE_BOOK_SIZE: %d, CODE_VECTOR_SIZE: %d*%d\n",
            width, height, CODE_BOOK_SIZE, CODE_VECTOR_X,CODE_VECTOR_Y);
    printf("Origian image size: %.2f kB. Compressed image size: %.2f kB.\nCompression ratio: %.2f%%\n", 
            (float)Size_Origin/1000, (float)Size_Compressed/1000, ratio);
    #endif

    return 0;
}
   
