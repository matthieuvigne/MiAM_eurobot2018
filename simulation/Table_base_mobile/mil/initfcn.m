function [] = initfcn(modelProperties)
%% Extract modelProperties fields value
listFields = fieldnames(modelProperties);
for ix = 1 : length(listFields)
    eval([listFields{ix},' = modelProperties.',listFields{ix},';'])
end
%% Clear Base workspace
evalin('base','clear')
%% Clear screen
clc;
%% Add init, lib & test paths
actualPath = cd;
for ix = 1 : length(listPath)
    try
        cd(listPath{ix});
        addpath(fullfile(cd,'init'));
        disp([fullfile(cd,'init'),' path added !']);
        addpath(fullfile(cd,'lib'));
        disp([fullfile(cd,'lib'),' path added !']);
    catch
        warning([listPath{ix},' path adding error...']);
    end
    cd(actualPath)
end
%% Add test directory
cd('..')
addpath(fullfile(cd,'test'));
disp([fullfile(cd,'test'),' path added !']);
cd(actualPath);
%% Add specific paths
for ix = 1 : length(listSpecificPath)
    cd(listSpecificPath{ix});
    addpath(cd);
    disp([cd,' path added !']);
    cd(actualPath)
end
%% Load model
load_system(modelName)
%% Scripts running
for ix = 1 : length(listScript)
    try
        evalin('base',listScript{ix});
        disp([listScript{ix},' done !']);
    catch
        warning([listScript{ix},' execution error...']);
    end
end
%% Gain of the Signal Builder
sbId = find_system(modelName,'MaskType','Sigbuilder block');
if ~isempty(sbId)
    [~,~,listSignals]=signalbuilder(sbId{1});
    for ix = 1 :length(listSignals)
        assignin('base',[listSignals{ix},'_Gain'],1);
    end
end
%%
evalin('base',['save ws_',modelName])
%% Open model
open_system(modelName)