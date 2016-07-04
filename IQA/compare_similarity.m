% N = Number of images
N=4;
% M = Number of cases per image
M=3;

% dimensions must be the same!
imgs_t = ['lena   ';'kodim08';'kodim19';'kodim23'];
imgs = cellstr(imgs_t);

cases_t = ['3-0.4';'5-1  ';'7-1.5'];
cases = cellstr(cases_t);

space = ' ';

% for each image
for i=1:N
    % read the reference image
    ref=imread(char(strcat(imgs(i),'_gray.bmp')));
    
    for j=1:M
        % read the OpenCV float-coefficients image
        float_name=strcat(imgs(i),'_blurred_cv_',cases(j),'.bmp');
        float=imread(char(float_name));
        disp(char(strcat('FSIM between --',float_name, '--and reference image: ')));
        fsim=FeatureSIM(ref, float);
        1-fsim
        
        % read the OpenCV integer-approximated-coefficients version
        int_appr_name=strcat(imgs(i),'_blurred_appr_',cases(j),'.bmp');
        int_appr=imread(char(int_appr_name));
        disp(char(strcat('FSIM between --', int_appr_name, '-- and --', float_name, '-- :')));
        fsim=FeatureSIM(float, int_appr);
        1-fsim
        
        % read the sepImageFilter version
        hw_name=strcat(imgs(i),'_hw_',cases(j),'.bmp');
        hw=imread(char(hw_name));
        disp(char(strcat('FSIM between --', hw_name, '-- and --', float_name, '-- :')));
        fsim=FeatureSIM(hw, float);
        1-fsim
    end
end


