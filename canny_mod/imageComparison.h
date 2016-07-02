
Scalar getMSSIM( const Mat& i1, const Mat& i2);
double getPSNR(const Mat& I1, const Mat& I2);

double sigma(Mat & m, int i, int j, int block_size);
double cov(Mat & m1, Mat & m2, int i, int j, int block_size);
double eqm(Mat & img1, Mat & img2);
double psnr(Mat & img_src, Mat & img_compressed);
double ssim(Mat & img_src, Mat & img_compressed, int block_size, bool show_progress);
