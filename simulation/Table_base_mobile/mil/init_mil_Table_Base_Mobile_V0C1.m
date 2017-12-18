function []=init_mil_Table_Base_Mobile_V0C1()
milName='mil_table_base_mobile_v0c1';
% xlsFileName = '';
% SBName = [milName,'/...'];
%% List of path
listPathModel = {...
   '..',...
   };

listPath = [
    listPathModel,...
    ];

listSpecificPath ={};


%% List of scripts
listScriptModel  = {...
    'startup_Contact_Forces',...
    'Table_Base_Mobile_V0C_DataFile',...
    };
listScriptSoft = {...
    };
%     ['updateSB(''',SBName,''',''',xlsFileName,''' );']...
    

listScript = [...
    listScriptModel,...
    listScriptSoft,...
    ];


modelProperties = [];
modelProperties.modelName = milName;
modelProperties.listPath = listPath;
modelProperties.listSpecificPath = listSpecificPath;
modelProperties.listScript = listScript;
initfcn(modelProperties);
save_system(milName);