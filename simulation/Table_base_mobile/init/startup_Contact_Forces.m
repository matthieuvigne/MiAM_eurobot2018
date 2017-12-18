% Startup for Simscape Multibody Contact Force Library
% Copyright 2014-2017 The MathWorks, Inc.

CFR_HomeDir = pwd;
addpath(genpath(pwd));

CFL_libname = 'Contact_Forces_Lib';
load_system(CFL_libname);
CFL_ver = get_param(CFL_libname,'Description');
disp(CFL_ver);
which(CFL_libname)


