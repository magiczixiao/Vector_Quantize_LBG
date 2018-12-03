/*********************
 * 解压缩codebook.raw
 * 参数设定:
 * 图像大小:512*512
 * 码书大小:32
 * 码字大小:8*8
 * 编译环境:Linux gcc
 * 
 * 请输入指令: source compile.sh 进行编译
 * 生成可执行文件: Decode_VQ
 * 输入指令: ./Decode_VQ 运行程序
 * 
 * 解压输入文件:codebook.raw
 * 解压输出文件:codebook.raw 
 * 
 * 王子潇 18120320
 * 2018.11.23
 * *********************/


#include<stdio.h>

#define __DEBUG

typedef unsigned char code_t;
typedef unsigned char index_t;

int main()
{
    printf("Begin decompressing...\n");
    FILE *fp_encode;
    FILE *fp_decode;
    int i, j, k;
    int CODE_BOOK_SIZE, CODE_VECTOR_X, CODE_VECTOR_SIZE, width, cols, parts,Size_Origin;
    fp_encode = fopen("codebook.raw", "rb");
    fp_decode = fopen("lena_decode.raw", "wb");
    
    fread(&CODE_BOOK_SIZE, sizeof(int), 1, fp_encode);//codebook.raw的第1字节读取到CODE_BOOK_SIZE中去
    fread(&CODE_VECTOR_X, sizeof(int), 1, fp_encode);//第2字节读取到CODE_VECTOR_X中去
    fread(&width, sizeof(int), 1, fp_encode);//第3字节:图像宽度(512)
    CODE_VECTOR_SIZE = CODE_VECTOR_X*CODE_VECTOR_X;
    cols = width / CODE_VECTOR_X;
    parts = cols * cols;
    Size_Origin = ftell(fp_encode);
    
    #ifdef __DEBUG 
    printf("width = %d\n",width);
    printf("rows = cols = %d\n",cols);
    printf("size of codebook.raw = %d\n",Size_Origin);
    printf("parts = %d\n",parts);
    #endif

    code_t codebook[CODE_BOOK_SIZE][CODE_VECTOR_SIZE];
    for(i = 0; i < CODE_BOOK_SIZE; i++)
    {
        for(j = 0; j < CODE_VECTOR_SIZE; j++)
        {
            fread(&codebook[i][j], sizeof(code_t), 1, fp_encode);//读取码书
        }
    }
    int decoded_data[width][width];//用于存储解压后的数据
    index_t index = 0;
    int re_i = 0, re_j = 0;
    for(int i = 0; i < 64; i++)//图像每一行有64个块
    {
        for(int j = 0; j < 64;j++)//每一列有64个块
        {
            fread(&index, sizeof(index_t), 1, fp_encode);//读取索引index,对照码书进行解压
            re_i = i*8;//记录是第几行
            re_j = j*8;//记录是第几列
            for (int ii = 0; ii < 8; ii++)//每一块共有8行
            {
                for(int jj = 0; jj<8; jj++)//每一块有8列
                {
                    int temp = 8*ii;
                    decoded_data[re_i+ii][re_j+jj] = codebook[index][temp+jj]; //codebook[index][0->63]
                    
                    #ifdef __DEBUG
                    printf("decoded_data[%d][%d] = codebook[%d][%d], ii=%d, jj=%d\n",
                    re_i+ii, re_j+jj,index,temp+jj,ii,jj);
                    //getchar();
                    #endif

                }
                //printf("HERE ii = %d",ii);
                //getchar();
            }
        }
    }
    for(i = 0; i < width; i++)
    {
        for(j = 0; j < width; j++)
        {
            fwrite(&decoded_data[i][j], 1, 1, fp_decode);//写入解压后的数据
        }
    }
    fclose(fp_encode);
    fclose(fp_decode);

    #ifdef __DEBUG
    printf("Decompression complete!\n");
    printf("\nImage size: %d*%d, CODE_BOOK_SIZE: %d, CODE_VECTOR_SIZE: %d*%d\n",
     width, width, CODE_BOOK_SIZE, CODE_VECTOR_X,CODE_VECTOR_X);
    #endif

    return 0;

}
