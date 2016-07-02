double sigma(Mat & m, int i, int j, int block_size);
double cov(Mat & m1, Mat & m2, int i, int j, int block_size);
double eqm(Mat & img1, Mat & img2);
double psnr(Mat & img_src, Mat & img_compressed, int block_size);
double ssim(Mat & img_src, Mat & img_compressed, int block_size, bool show_progress = false);
void compute_quality_metrics(char * file1, char * file2, int block_size);
