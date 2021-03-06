% N = Algorithm versions
N=4;


ref='lena_secret.bmp';
img_ref=imread(ref);
ref_gray='lena_gray.bmp';
img_ref_gray=imread(ref_gray);

%blur0='lena_blurred_0.bmp';
%blur1='lena_blurred_1.bmp';
%blur2='lena_blurred_2.bmp';
%blur3='lena_blurred_3.bmp';
%blur4='lena_blurred_4.bmp';
%blur5='lena_blurred_5.bmp';
%blur6='lena_blurred_6.bmp';
%img_blur0=imread(blur0);
%img_blur1=imread(blur1);
%img_blur2=imread(blur2);
%img_blur3=imread(blur3);
%img_blur4=imread(blur4);
%img_blur5=imread(blur5);
%img_blur6=imread(blur6);

% strings cannot be stored inside matrices or arrays, so we must create a
% cell array
blurred = cell(N,1);
img=uint8(zeros(512,512,N));
fsims=zeros(N,1);
for j=1:N
    blurred{j}=strcat('src_blurred_', num2str(j-1), '.bmp');
    img(:,:,j)=imread(char(blurred{j}));
    %imshow(img(:,:,j))
    %a=input('go..');
    %disp(strcat('FSIM between ref_gray and alg ', num2str(i), ':'))
    fsims(j,1)=FeatureSIM(img_ref_gray, img(:,:,j));
end

fsims
min(fsims)
find(fsims == min(fsims))

FeatureSIM(img(:,:,1), img(:,:,3))
