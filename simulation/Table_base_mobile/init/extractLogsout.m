function [] = extractLogsout()
logsout = evalin('base','logsout'); % Copy logsout in the Base Workspace to Caller Workspace
listData = get(logsout); % Get logged signals list
%%
for ix = 1 : length(listData)
    try
        evalin('base',['clear ',listData{ix}]); % Clear the variable in the Base Workspace
    end
    dataToExtract = get(logsout,listData{ix}); % Extract the ix-th data
    try
        extractData(listData{ix},dataToExtract);
    catch
        nmbrOfElements = numElements(dataToExtract); % Catch in case of different signals are stored with the same name apply a suffixe to the stored data
        for jx = 1 : nmbrOfElements
            dataToExtractTemp = dataToExtract.getElement(jx);
            extractData([listData{ix},'_',num2str(jx)],dataToExtractTemp);
        end
    end
end
end
function [] = extractData(signalToExtractName,dataSignalSimulationToExtract)
assignin('base',[signalToExtractName,'_SimData'],dataSignalSimulationToExtract); % Store the Simulation Data of the signal in an indicative variable
assignin('base',signalToExtractName,dataSignalSimulationToExtract.Values); % Extract the signal data simulation in a timeseries format
end