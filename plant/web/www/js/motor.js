//-------------Node -----------------------
function getNodeList(pageNo) {
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD : "AllNodeList",
        PAGENO : pageNo
    },
    function(data) {
    				//dealPage(data);
        dealNodeList(data);
    });
}
//function dealPage(data){
//$(data).find('page').each(function() {
//        var pageNo = parseInt($(this).children('pageno').text());
//        var maxPage = parseInt($(this).children('maxpage').text());
//        $('#JQpagination').jqPagination('option', {current_page:pageNo,max_page:maxPage},true);
//        return false;
//    });	
//	}
function getNodeListPage(pageNo) {

    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD : "AllNodeList",
        PAGENO : pageNo
    },
    function(data) {
        dealNodeList(data);
    });
}	
	
function dealNodeList(data) {
    var html = "";
    
    $(data).find('node').each(function() {
		var id = $(this).children('id').text();
        if ($(this).children('isValid').text() == "0") {
            html += "<div class='item inActivity' onclick='showNodeInfo(" + id + ");'>";
        } else {
            html += "<div class='item' onclick='showNodeInfo(" + id + ");'>";
        }
		html += "<input onclick='preventEvent(event);' id='isNodeSelected"+ id +"' name='isNodeSelected' type='checkbox' value='"+ id+"' />";
        html += "<label class='nodes'>"+$(this).children('memo').text();
        html += "</label><label class='areaName'>" + $(this).children('areaname').text() + "</label></div>";

    }); 
    html += ""; 
    $("#NODELIST").html(html);
    
    $("#selectAll").prop("checked",false);

}



function initPagination() {
    $('#JQpagination').jqPagination({
        page_string:'Page {current_page} of {max_page} ',
        paged: function(page) {
            currentPage = page;
            	getNodeListPage(page);
        }
    });
}

function getAreaList() {
    var loc = "AreaManage.action";
    $.get(loc, {
        METHOD: "AreaList"
    },
    function(data) {

        dealSelector(data);

    });
}
function dealSelector(data) {
    var selObj = $("#nodeArea");
    $(data).find('domain').each(function() {
        selObj.append("<option value='" + $(this).children('id').text() + "'>" + $(this).children('name').text() + "</option>");

    });
}

function showNodeInfo(nodeId) {
			
			$("#isNodeSelected"+nodeId).prop("checked",true);	
	
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "NodeInfo",
        NODEID: nodeId
    },
    function(data) {
        dealNodeInfo(data);
    });
}

function dealNodeInfo(data) {
    $(data).find('node').each(function() {
        $("#nodeId").val($(this).children('id').text());
        $("#nodeMac").val($(this).children('mac').text());
        $("#memo").val($(this).children('memo').text());
        $("#nodeArea").val($(this).children('domain').text());
        return false;
    });

}

function autoSearch() {
	$("#AutoSearchMode").dialog('open');
}
function doAautoSearch(){
	$("#BTN_autoSearch").attr('disabled',"true");
	$("#AutoSearchMode").dialog('close');
	
	var isReset = 0;
	if($("#isResetCOO").prop("checked")){
		isReset = 1;
	}
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "AutoSearch",
							ISRESET:isReset
    },
    function(data) {
		$("#AutoSearchDialog").dialog( 'close' );
    	  $("#BTN_autoSearch").removeAttr("disabled");	
        if (dealReturnMessage(data, "Auto search") > 0) {
            getNodeList(1);
        }
      
    });
    $(  "#AutoSearchDialog" ).dialog( 'open' );
}
function initComp(){
	$("#AutoSearchDialog").dialog({
        autoOpen: false,
        height: 180,
        width: 200,
        modal: true,
        resizable: false
    });
 	$("#AutoSearchMode").dialog({
        autoOpen: false,
        height: 230,
        width: 350,
        modal: true,
        resizable: false,
		buttons: {
            "OK": function() {
                doAautoSearch();
            },

            "Cancel": function() {
                $(this).dialog("close");
            }
        }
    }); 
    
}

function newNode() {
    $("#nodeId").val("-1");
    $("#nodeMac").val("");
    $("#memo").val("");
    $("#nodeArea").val(0);
    $("#nodeMac").focus();
}

function saveNode(objButton) {
	$(objButton).attr('disabled',"true");
    var nodeID = $("#nodeId").val();
    var memo = $("#memo").val();
    var nodeArea = $("#nodeArea").val();
    var nodeMac = $("#nodeMac").val();

    if (memo == "") {
        alert("Memo please .");
        $("#memo").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }
    if (nodeMac.length != 8) {
        alert("MAC's length should be 8 bytes.");
        $("#nodeMac").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }

    var reg = /^[0-9a-fA-F]+$/;
    if (!reg.test(nodeMac)) {
        alert("MAC is incorrect,please input again.");
        $("#nodeMac").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }

    saveNodeDB(nodeID, memo, nodeArea, nodeMac,objButton);
    
}
function saveNodeDB(nodeID, memo, nodeArea, nodeMac,objButton) {
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "SaveNode",
        NODEID: nodeID,
        MEMO: memo,
        NODEAREA: nodeArea,
        NODEMAC: nodeMac
    },
    function(data) {
    	$(objButton).removeAttr("disabled");	
    	var returnCode = dealReturnMessage_eM(data, "Save node ","The nodes has been saved already or the device with this MAC is not exist.");
    	if(returnCode>0){
        $("#nodeId").val(returnCode);
        getNodeList(	currentPage);
     }
        

    });
}
function deleteNode() {
   // var nodeID = $("#nodeId").val();
   var nodeIDs = "";
   var count = 0;
	$("[name='isNodeSelected']").each(function(){
				if($(this).prop("checked")){
					nodeIDs += $(this).val()+",";
					count++;
				}
	});
	if(count==0){
		alert("please select one node at least.");
		return false;
	}
	
	if(!confirm("You are deleting "+count+" nodes,aren't you?")){
		return false;
	}
	
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "DeleteNode",
        NODEID: nodeIDs
    },
    function(data) {
        if (dealReturnMessage(data, "Delete nodes") > 0) {
            newNode();
            getNodeList(currentPage);
        }
    });
}


function selectAllNode(obj){
		var isSelected = $(obj).prop("checked");
		$("[name='isNodeSelected']").each(function(){
				$(this).prop("checked",isSelected);
			});
	}
	

	

