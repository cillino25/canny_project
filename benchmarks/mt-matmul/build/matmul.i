# 1 "../matmul.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "../matmul.c"


# 1 "../dataset.h" 1
# 10 "../dataset.h"
typedef double data_t;

static data_t input1_data[1024] =
{
    0, 3, 2, 0, 3, 1, 0, 3, 2, 3, 2, 0, 3, 3, 1, 2, 3, 0, 0, 1,
    1, 1, 2, 3, 1, 2, 3, 1, 1, 3, 2, 2, 0, 1, 3, 2, 2, 2, 0, 0,
    1, 0, 1, 3, 3, 0, 3, 3, 3, 3, 0, 3, 2, 1, 2, 2, 0, 0, 3, 0,
    1, 1, 0, 3, 3, 1, 2, 3, 3, 0, 1, 2, 1, 0, 1, 2, 2, 1, 0, 3,
    1, 0, 2, 2, 1, 1, 1, 1, 1, 1, 2, 0, 3, 1, 1, 2, 2, 3, 3, 1,
    3, 2, 0, 0, 0, 3, 3, 3, 2, 1, 2, 3, 1, 0, 0, 0, 0, 1, 2, 2,
    1, 1, 3, 3, 3, 1, 1, 2, 3, 1, 3, 3, 2, 3, 2, 1, 2, 3, 0, 2,
    2, 1, 1, 0, 0, 0, 0, 0, 1, 3, 3, 1, 1, 1, 2, 2, 3, 2, 1, 1,
    1, 1, 3, 0, 2, 2, 1, 3, 2, 1, 2, 2, 1, 3, 1, 3, 1, 3, 2, 3,
    1, 2, 1, 3, 2, 2, 0, 1, 0, 0, 1, 2, 3, 3, 1, 0, 0, 0, 3, 1,
    2, 3, 2, 3, 2, 0, 0, 0, 0, 0, 3, 1, 3, 0, 0, 0, 3, 1, 1, 1,
    1, 2, 1, 2, 3, 2, 0, 0, 2, 2, 3, 0, 3, 0, 0, 3, 0, 3, 1, 3,
    3, 1, 1, 1, 2, 2, 1, 3, 0, 3, 3, 1, 0, 0, 3, 2, 1, 3, 3, 3,
    1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 2, 3, 1, 2, 2, 2, 0, 1, 3, 3,
    3, 2, 2, 1, 0, 1, 2, 0, 1, 1, 1, 1, 2, 3, 2, 2, 3, 3, 0, 0,
    2, 0, 0, 0, 3, 0, 1, 0, 3, 0, 0, 0, 3, 0, 0, 2, 0, 2, 0, 0,
    2, 3, 2, 0, 0, 3, 3, 2, 1, 1, 0, 2, 0, 0, 3, 3, 2, 3, 3, 0,
    1, 0, 2, 2, 0, 3, 3, 1, 1, 0, 2, 3, 2, 1, 1, 0, 1, 2, 1, 2,
    2, 0, 0, 1, 0, 1, 1, 0, 1, 0, 2, 3, 3, 2, 0, 0, 1, 3, 0, 3,
    3, 0, 0, 0, 0, 3, 3, 1, 0, 0, 3, 3, 2, 1, 2, 1, 3, 3, 0, 1,
    3, 0, 2, 3, 1, 3, 3, 3, 3, 3, 0, 1, 1, 3, 0, 2, 2, 3, 1, 2,
    2, 2, 1, 3, 3, 0, 3, 0, 0, 2, 0, 2, 3, 0, 1, 3, 2, 2, 0, 0,
    2, 3, 0, 2, 2, 2, 3, 1, 0, 3, 3, 3, 3, 1, 0, 3, 3, 2, 0, 3,
    2, 0, 3, 0, 2, 0, 0, 2, 2, 1, 0, 2, 3, 1, 1, 1, 1, 2, 3, 3,
    3, 0, 0, 3, 3, 3, 2, 3, 3, 1, 2, 2, 3, 1, 2, 1, 1, 3, 0, 1,
    2, 0, 2, 0, 0, 1, 3, 2, 0, 1, 3, 2, 3, 3, 0, 0, 0, 1, 0, 3,
    3, 2, 2, 2, 1, 1, 2, 2, 1, 3, 2, 0, 1, 3, 2, 0, 2, 1, 3, 0,
    0, 0, 1, 3, 3, 2, 2, 2, 3, 1, 0, 0, 1, 1, 2, 1, 3, 1, 1, 2,
    2, 3, 2, 3, 0, 2, 3, 3, 0, 3, 0, 0, 1, 0, 0, 0, 1, 3, 1, 1,
    2, 3, 2, 1, 1, 2, 2, 2, 3, 0, 1, 1, 2, 1, 2, 0, 2, 3, 1, 3,
    0, 1, 1, 3, 0, 2, 3, 0, 1, 2, 3, 2, 0, 0, 3, 3, 2, 1, 1, 2,
    3, 0, 1, 1, 1, 1, 2, 0, 1, 2, 0, 1, 1, 1, 0, 1, 3, 2, 3, 1,
    0, 2, 1, 2, 1, 3, 3, 1, 0, 2, 2, 3, 1, 3, 1, 3, 0, 1, 0, 3,
    0, 3, 2, 0, 3, 3, 3, 0, 3, 2, 2, 2, 1, 3, 0, 0, 1, 1, 3, 0,
    1, 2, 1, 0, 0, 0, 3, 2, 2, 0, 0, 2, 1, 3, 0, 0, 3, 0, 0, 2,
    1, 1, 2, 2, 1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 1, 1, 3, 0, 1, 3,
    3, 2, 2, 1, 0, 3, 2, 2, 2, 3, 0, 1, 3, 3, 2, 3, 0, 3, 2, 3,
    1, 1, 0, 0, 0, 2, 3, 0, 3, 0, 1, 1, 3, 1, 3, 2, 1, 1, 2, 1,
    3, 2, 0, 2, 1, 0, 2, 3, 2, 3, 2, 1, 2, 3, 0, 0, 1, 1, 0, 0,
    2, 1, 0, 1, 2, 2, 2, 2, 0, 3, 3, 1, 0, 0, 0, 0, 3, 1, 1, 0,
    0, 0, 0, 1, 2, 2, 1, 3, 0, 2, 3, 2, 3, 2, 2, 1, 2, 2, 3, 3,
    1, 3, 0, 2, 2, 3, 3, 1, 2, 2, 2, 3, 1, 1, 1, 0, 0, 0, 3, 0,
    1, 0, 3, 1, 1, 3, 0, 1, 2, 2, 0, 0, 3, 3, 3, 3, 2, 1, 0, 0,
    1, 0, 2, 0, 1, 1, 0, 0, 3, 3, 2, 1, 1, 1, 0, 1, 1, 2, 2, 1,
    1, 2, 0, 3, 1, 3, 1, 0, 3, 0, 3, 1, 1, 1, 0, 2, 0, 3, 1, 0,
    1, 0, 2, 0, 2, 3, 3, 3, 1, 2, 3, 2, 2, 0, 1, 1, 0, 3, 3, 1,
    3, 3, 2, 0, 2, 0, 2, 2, 3, 3, 3, 0, 2, 3, 3, 1, 3, 2, 2, 2,
    0, 2, 3, 0, 2, 0, 3, 2, 2, 1, 1, 0, 2, 2, 2, 0, 2, 2, 0, 1,
    3, 2, 1, 3, 2, 2, 0, 3, 3, 1, 2, 2, 0, 0, 3, 2, 1, 2, 2, 1,
    3, 1, 2, 0, 0, 1, 1, 2, 1, 3, 2, 2, 3, 0, 2, 1, 3, 2, 1, 3,
    2, 3, 3, 1, 2, 1, 2, 2, 0, 0, 0, 3, 0, 2, 3, 1, 0, 0, 2, 3,
    3, 2, 2, 1
};

static data_t input2_data[1024] =
{
    1, 1, 0, 3, 1, 2, 0, 0, 0, 0, 0, 2, 1, 2, 3, 0, 0, 3, 3, 2,
    2, 1, 2, 3, 3, 0, 2, 2, 1, 1, 2, 2, 0, 2, 2, 1, 2, 3, 2, 2,
    3, 3, 2, 2, 1, 1, 1, 1, 2, 1, 2, 2, 3, 3, 3, 0, 0, 3, 2, 3,
    2, 3, 1, 2, 1, 1, 2, 2, 0, 1, 0, 3, 2, 1, 1, 1, 2, 0, 1, 2,
    2, 0, 2, 1, 3, 3, 2, 3, 2, 0, 3, 1, 3, 3, 2, 0, 1, 0, 1, 1,
    2, 2, 1, 1, 2, 2, 1, 2, 3, 3, 1, 3, 2, 2, 2, 3, 3, 1, 0, 2,
    1, 0, 0, 0, 1, 1, 2, 0, 3, 2, 3, 3, 0, 2, 3, 1, 0, 0, 2, 1,
    2, 0, 2, 1, 1, 2, 3, 1, 3, 2, 1, 0, 0, 0, 0, 0, 2, 2, 0, 2,
    1, 2, 0, 3, 2, 2, 0, 0, 3, 2, 1, 1, 3, 0, 2, 0, 0, 1, 0, 2,
    3, 3, 1, 3, 3, 0, 0, 2, 2, 0, 0, 0, 1, 0, 0, 1, 3, 0, 2, 1,
    3, 2, 2, 1, 3, 2, 0, 1, 2, 2, 3, 2, 1, 1, 1, 1, 3, 0, 1, 3,
    2, 2, 3, 1, 1, 2, 0, 2, 1, 1, 2, 3, 1, 0, 1, 0, 1, 1, 0, 0,
    2, 0, 3, 0, 3, 0, 3, 2, 2, 3, 3, 2, 1, 0, 2, 2, 1, 1, 0, 3,
    3, 2, 2, 0, 0, 3, 0, 1, 0, 0, 1, 2, 0, 1, 3, 0, 1, 2, 2, 0,
    0, 3, 0, 3, 0, 1, 1, 2, 0, 0, 0, 3, 0, 0, 2, 1, 1, 1, 0, 2,
    1, 3, 1, 2, 0, 3, 0, 3, 1, 3, 0, 0, 2, 2, 2, 2, 3, 3, 2, 1,
    2, 2, 1, 1, 2, 2, 2, 2, 0, 3, 0, 0, 2, 0, 1, 2, 0, 3, 2, 3,
    2, 0, 2, 1, 2, 1, 0, 2, 1, 1, 3, 2, 2, 3, 1, 0, 3, 3, 1, 0,
    3, 2, 2, 0, 0, 3, 0, 0, 2, 0, 3, 2, 3, 1, 1, 0, 0, 2, 3, 0,
    0, 1, 1, 1, 2, 1, 3, 2, 1, 3, 0, 1, 3, 3, 1, 1, 1, 1, 1, 1,
    0, 0, 2, 3, 2, 2, 2, 3, 2, 3, 1, 2, 3, 2, 2, 2, 0, 1, 3, 0,
    1, 1, 0, 1, 0, 1, 1, 3, 3, 1, 2, 2, 3, 2, 0, 2, 2, 0, 1, 3,
    0, 1, 3, 2, 1, 3, 3, 2, 0, 1, 3, 2, 0, 2, 1, 1, 0, 3, 0, 1,
    1, 1, 1, 1, 3, 0, 0, 1, 0, 2, 3, 1, 3, 0, 2, 1, 3, 0, 3, 0,
    3, 2, 2, 0, 0, 2, 1, 3, 3, 2, 3, 2, 2, 1, 2, 2, 3, 0, 3, 2,
    2, 0, 3, 2, 3, 2, 0, 0, 1, 2, 0, 0, 2, 0, 0, 3, 3, 2, 0, 0,
    3, 3, 0, 2, 3, 1, 0, 1, 0, 2, 1, 0, 2, 1, 0, 1, 0, 3, 0, 2,
    2, 3, 0, 0, 2, 1, 0, 1, 0, 0, 0, 2, 2, 3, 2, 0, 3, 3, 2, 1,
    0, 0, 3, 1, 2, 3, 3, 1, 0, 3, 1, 1, 0, 3, 3, 3, 2, 2, 2, 0,
    1, 2, 0, 3, 0, 1, 0, 1, 1, 0, 1, 2, 0, 3, 2, 0, 1, 2, 2, 0,
    2, 0, 0, 1, 0, 3, 0, 3, 2, 1, 1, 1, 1, 3, 2, 1, 1, 1, 1, 0,
    2, 1, 1, 3, 2, 0, 2, 1, 1, 0, 2, 2, 1, 3, 0, 2, 1, 0, 1, 2,
    0, 1, 3, 2, 3, 2, 1, 0, 2, 0, 2, 2, 3, 1, 1, 3, 2, 3, 2, 2,
    0, 2, 0, 0, 0, 3, 2, 0, 2, 2, 3, 3, 3, 2, 1, 2, 0, 0, 3, 0,
    2, 0, 3, 2, 2, 3, 0, 3, 2, 1, 2, 2, 1, 2, 0, 0, 3, 1, 2, 0,
    2, 3, 2, 2, 1, 1, 1, 3, 3, 3, 3, 3, 1, 3, 0, 1, 3, 2, 2, 1,
    0, 1, 1, 2, 1, 2, 3, 1, 2, 2, 1, 2, 1, 1, 0, 3, 3, 1, 1, 3,
    2, 0, 0, 1, 2, 0, 1, 3, 1, 0, 0, 2, 2, 3, 3, 0, 2, 3, 2, 1,
    1, 3, 0, 2, 2, 3, 3, 1, 2, 3, 3, 3, 1, 3, 0, 3, 1, 1, 2, 2,
    2, 1, 0, 3, 2, 3, 0, 2, 3, 2, 3, 1, 2, 3, 3, 1, 2, 1, 0, 0,
    0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 2, 1, 0, 3, 0, 3, 2, 3, 1, 0,
    0, 1, 3, 1, 0, 2, 2, 3, 1, 0, 2, 1, 1, 3, 1, 1, 3, 1, 2, 1,
    0, 0, 3, 2, 1, 1, 1, 1, 3, 2, 1, 3, 3, 1, 0, 3, 1, 1, 2, 0,
    0, 0, 2, 3, 3, 2, 2, 3, 0, 2, 3, 1, 3, 3, 0, 2, 1, 2, 2, 2,
    1, 0, 1, 3, 2, 3, 1, 1, 2, 1, 1, 0, 0, 2, 3, 2, 1, 0, 3, 1,
    3, 0, 1, 1, 2, 2, 1, 3, 3, 1, 1, 0, 0, 3, 3, 0, 0, 0, 0, 0,
    3, 1, 3, 0, 0, 0, 3, 3, 2, 1, 3, 0, 1, 3, 1, 1, 1, 0, 1, 0,
    1, 2, 2, 2, 3, 3, 0, 2, 3, 2, 1, 3, 3, 1, 1, 3, 0, 3, 3, 2,
    1, 1, 2, 0, 3, 0, 1, 2, 1, 1, 0, 0, 1, 2, 2, 0, 3, 1, 1, 1,
    3, 3, 3, 1, 0, 3, 3, 2, 2, 2, 1, 2, 0, 1, 1, 3, 0, 3, 1, 0,
    2, 2, 0, 1, 2, 3, 2, 1, 2, 0, 3, 2, 1, 3, 0, 1, 2, 0, 3, 0,
    1, 1, 2, 1
};

static data_t verify_data[1024] =
{
   72, 75, 88, 101, 80, 88, 73, 75, 80, 81, 58, 75, 86, 65, 60, 80, 84, 83, 87, 83,
  108, 93, 85, 76, 72, 98, 79, 86, 80, 96, 91, 85, 72, 64, 70, 83, 68, 92, 51, 54,
   85, 85, 60, 58, 90, 64, 55, 69, 72, 48, 94, 77, 91, 83, 70, 69, 67, 77, 59, 50,
   67, 74, 77, 67, 67, 62, 72, 71, 68, 79, 54, 61, 67, 61, 55, 62, 78, 60, 53, 64,
   67, 69, 99, 68, 88, 60, 66, 63, 70, 62, 65, 50, 53, 66, 70, 72, 75, 78, 85, 95,
   71, 89, 70, 68, 86, 88, 58, 77, 84, 70, 65, 68, 73, 75, 91, 96, 105, 92, 76, 68,
   86, 69, 80, 59, 73, 83, 88, 75, 64, 63, 71, 99, 77, 77, 69, 55, 80, 73, 54, 73,
   87, 78, 60, 69, 65, 78, 86, 89, 95, 92, 63, 69, 89, 61, 80, 65, 70, 77, 89, 77,
   79, 79, 73, 92, 64, 81, 60, 78, 81, 80, 61, 63, 89, 65, 56, 83, 77, 65, 102, 70,
   98, 86, 96, 68, 72, 89, 73, 73, 70, 89, 84, 76, 48, 61, 63, 70, 70, 79, 50, 53,
   64, 63, 43, 51, 59, 62, 43, 63, 55, 77, 79, 74, 75, 74, 64, 44, 65, 69, 72, 66,
   54, 71, 74, 72, 69, 76, 68, 89, 94, 75, 65, 53, 85, 79, 65, 74, 82, 73, 58, 70,
   84, 77, 99, 72, 92, 84, 78, 62, 59, 83, 71, 74, 63, 85, 80, 78, 71, 72, 79, 83,
   73, 82, 60, 85, 76, 82, 60, 70, 82, 68, 54, 85, 84, 70, 86, 74, 100, 88, 98, 68,
   67, 87, 69, 73, 68, 88, 76, 71, 47, 43, 47, 80, 54, 65, 40, 37, 59, 53, 33, 48,
   62, 40, 36, 55, 36, 62, 53, 57, 70, 69, 45, 43, 53, 61, 42, 57, 56, 63, 51, 47,
   59, 75, 64, 89, 83, 75, 59, 75, 91, 92, 58, 64, 83, 74, 58, 60, 76, 66, 97, 69,
   90, 95, 92, 64, 78, 75, 77, 73, 65, 78, 82, 75, 47, 54, 59, 71, 59, 56, 53, 42,
   60, 55, 40, 51, 60, 46, 36, 59, 46, 57, 67, 43, 51, 53, 53, 38, 54, 56, 55, 48,
   41, 46, 63, 63, 80, 77, 89, 102, 89, 98, 74, 86, 98, 93, 63, 76, 98, 77, 48, 101,
   86, 88, 100, 82, 102, 90, 95, 75, 86, 103, 83, 98, 80, 104, 98, 86, 71, 74, 80, 90,
   86, 87, 73, 70, 81, 83, 55, 66, 90, 66, 58, 84, 77, 84, 93, 72, 99, 75, 85, 65,
   70, 89, 71, 82, 64, 79, 82, 80, 67, 73, 86, 101, 78, 97, 66, 64, 84, 80, 55, 64,
   79, 73, 51, 79, 89, 68, 94, 77, 109, 102, 82, 61, 66, 93, 88, 70, 82, 82, 85, 69,
   69, 72, 66, 97, 85, 90, 70, 59, 76, 89, 53, 56, 90, 79, 71, 64, 70, 67, 100, 92,
  106, 89, 83, 78, 73, 80, 70, 72, 65, 70, 92, 88, 57, 76, 55, 85, 66, 80, 61, 63,
   63, 78, 54, 58, 71, 73, 54, 63, 63, 62, 89, 76, 86, 81, 83, 54, 70, 81, 78, 64,
   56, 72, 74, 81, 75, 63, 68, 89, 65, 77, 58, 68, 75, 83, 52, 62, 82, 63, 55, 75,
   51, 70, 95, 66, 83, 77, 86, 61, 64, 77, 48, 70, 66, 82, 72, 75, 79, 71, 72, 89,
   78, 78, 66, 59, 91, 80, 55, 64, 79, 68, 54, 71, 67, 75, 87, 84, 100, 101, 76, 58,
   74, 82, 61, 74, 75, 97, 85, 79, 61, 55, 69, 68, 72, 65, 52, 64, 80, 73, 48, 54,
   71, 66, 42, 61, 66, 63, 92, 64, 85, 77, 73, 54, 74, 73, 76, 66, 62, 79, 85, 70,
   71, 84, 87, 81, 88, 86, 77, 77, 93, 88, 78, 71, 101, 89, 58, 84, 95, 81, 89, 97,
  104, 79, 83, 76, 90, 81, 91, 74, 70, 76, 91, 80, 51, 48, 56, 69, 47, 63, 54, 42,
   63, 63, 42, 52, 66, 56, 39, 59, 61, 52, 59, 63, 62, 68, 57, 35, 67, 58, 56, 52,
   61, 63, 60, 47, 85, 75, 89, 106, 88, 95, 74, 82, 107, 107, 64, 78, 98, 90, 62, 91,
   79, 87, 111, 84, 104, 106, 96, 68, 94, 99, 81, 89, 79, 105, 95, 86, 65, 63, 77, 89,
   66, 88, 56, 73, 82, 92, 41, 62, 85, 66, 50, 81, 57, 71, 77, 78, 86, 89, 77, 53,
   67, 78, 61, 63, 72, 82, 69, 66, 59, 46, 55, 70, 56, 64, 45, 50, 65, 64, 42, 56,
   78, 49, 51, 52, 38, 56, 72, 55, 73, 72, 61, 50, 63, 60, 47, 57, 55, 73, 53, 68,
   85, 88, 91, 96, 82, 89, 73, 76, 87, 86, 67, 69, 96, 84, 57, 89, 87, 89, 99, 88,
  104, 90, 85, 75, 88, 92, 85, 75, 74, 87, 103, 94, 55, 48, 56, 65, 72, 50, 45, 51,
   63, 62, 47, 57, 79, 53, 36, 63, 54, 68, 71, 59, 63, 61, 63, 41, 50, 73, 57, 59,
   56, 76, 73, 65, 61, 64, 61, 79, 53, 73, 57, 44, 61, 59, 59, 56, 81, 59, 49, 62,
   65, 55, 69, 72, 79, 70, 58, 57, 68, 61, 62, 50, 57, 60, 66, 66, 63, 77, 81, 89,
   85, 81, 76, 73, 78, 95, 59, 70, 81, 77, 46, 79, 78, 79, 83, 81, 84, 82, 85, 48,
   74, 85, 85, 74, 74, 80, 80, 74, 60, 76, 80, 97, 88, 93, 66, 66, 73, 84, 56, 70,
   90, 63, 58, 78, 73, 93, 90, 78, 94, 88, 82, 67, 85, 70, 81, 86, 74, 82, 88, 82,
   68, 73, 75, 91, 78, 97, 71, 66, 74, 85, 50, 59, 86, 77, 70, 74, 75, 74, 99, 82,
   99, 91, 86, 65, 80, 77, 72, 69, 60, 78, 90, 87, 79, 69, 74, 98, 70, 86, 81, 67,
   69, 78, 48, 65, 88, 70, 70, 70, 69, 72, 96, 90, 99, 82, 81, 76, 98, 73, 74, 71,
   69, 73, 94, 89
};
# 4 "../matmul.c" 2
# 1 "../../common/util.h" 1
# 33 "../../common/util.h"
extern void setStats(int enable);


# 1 "/usr/share/riscv_src/riscv/lib/gcc/riscv64-unknown-elf/5.3.0/include/stdint.h" 1 3 4
# 9 "/usr/share/riscv_src/riscv/lib/gcc/riscv64-unknown-elf/5.3.0/include/stdint.h" 3 4
# 1 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 1 3 4
# 12 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
# 1 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 1 3 4







# 1 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/sys/features.h" 1 3 4
# 9 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 2 3 4
# 27 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4

# 27 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef signed char __int8_t;

typedef unsigned char __uint8_t;
# 41 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef short int __int16_t;

typedef short unsigned int __uint16_t;
# 63 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef int __int32_t;

typedef unsigned int __uint32_t;
# 89 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef long int __int64_t;

typedef long unsigned int __uint64_t;
# 120 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef signed char __int_least8_t;

typedef unsigned char __uint_least8_t;
# 146 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef short int __int_least16_t;

typedef short unsigned int __uint_least16_t;
# 168 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef int __int_least32_t;

typedef unsigned int __uint_least32_t;
# 186 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef long int __int_least64_t;

typedef long unsigned int __uint_least64_t;
# 200 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/machine/_default_types.h" 3 4
typedef long int __intptr_t;

typedef long unsigned int __uintptr_t;
# 13 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 2 3 4
# 1 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/sys/_intsup.h" 1 3 4
# 14 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 2 3 4






typedef __int8_t int8_t ;
typedef __uint8_t uint8_t ;




typedef __int_least8_t int_least8_t;
typedef __uint_least8_t uint_least8_t;




typedef __int16_t int16_t ;
typedef __uint16_t uint16_t ;




typedef __int_least16_t int_least16_t;
typedef __uint_least16_t uint_least16_t;




typedef __int32_t int32_t ;
typedef __uint32_t uint32_t ;




typedef __int_least32_t int_least32_t;
typedef __uint_least32_t uint_least32_t;




typedef __int64_t int64_t ;
typedef __uint64_t uint64_t ;




typedef __int_least64_t int_least64_t;
typedef __uint_least64_t uint_least64_t;
# 74 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef int int_fast8_t;
  typedef unsigned int uint_fast8_t;
# 84 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef int int_fast16_t;
  typedef unsigned int uint_fast16_t;
# 94 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef int int_fast32_t;
  typedef unsigned int uint_fast32_t;
# 104 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef long int int_fast64_t;
  typedef long unsigned int uint_fast64_t;
# 153 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef long int intmax_t;
# 162 "/usr/share/riscv_src/riscv/riscv64-unknown-elf/include/stdint.h" 3 4
  typedef long unsigned int uintmax_t;






typedef __intptr_t intptr_t;
typedef __uintptr_t uintptr_t;
# 10 "/usr/share/riscv_src/riscv/lib/gcc/riscv64-unknown-elf/5.3.0/include/stdint.h" 2 3 4
# 37 "../../common/util.h" 2


# 38 "../../common/util.h"
extern int have_vec;



static void printArray(const char name[], int n, const int arr[])
{







}

static void printDoubleArray(const char name[], int n, const double arr[])
{







}

static int verify(int n, const volatile int* test, const int* verify)
{
  int i;

  for (i = 0; i < n/2*2; i+=2)
  {
    int t0 = test[i], t1 = test[i+1];
    int v0 = verify[i], v1 = verify[i+1];
    if (t0 != v0) return i+1;
    if (t1 != v1) return i+2;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static int verifyDouble(int n, const volatile double* test, const double* verify)
{
  int i;

  for (i = 0; i < n/2*2; i+=2)
  {
    double t0 = test[i], t1 = test[i+1];
    double v0 = verify[i], v1 = verify[i+1];
    int eq1 = t0 == v0, eq2 = t1 == v1;
    if (!(eq1 & eq2)) return i+1+eq1;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static void __attribute__((noinline)) barrier(int ncores)
{
  static volatile int sense;
  static volatile int count;
  static __thread int threadsense;

  __sync_synchronize();

  threadsense = !threadsense;
  if (__sync_fetch_and_add(&count, 1) == ncores-1)
  {
    count = 0;
    sense = threadsense;
  }
  else while(sense != threadsense)
    ;

  __sync_synchronize();
}

static uint64_t lfsr(uint64_t x)
{
  uint64_t bit = (x ^ (x >> 1)) & 1;
  return (x >> 1) | (bit << 62);
}


# 1 "../../common/encoding.h" 1
# 124 "../../common/util.h" 2
# 5 "../matmul.c" 2

void __attribute__((noinline)) matmul(const int coreid, const int ncores, const int lda, const data_t A[], const data_t B[], data_t C[] )
{
   int i, j, k;
   int block = lda / ncores;
   int start = block * coreid;

   for ( j = start; j < (start+block); j++ )
   {
      for ( k = 0; k < lda; k++ )
      {
         for ( i = 0; i < lda; i++ )
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }
   }
}