function doSysUpdate(isW3) {
// alert(isW3);
	var loc = "SystemManage.action";
	var url = $("#updateUrl").val();

	$.get(loc, {
		METHOD: "SystemUpdate",
		ISW3UPDATE: isW3,
		URL:url
	}, function(data) {
		$("#WaitingDialog").dialog('close');
		enableAllBtn();
		$(data).find('return').each( function ()  
		{ 	
			var code = parseInt($(this).children('code').text());

				if(code > 0){
					alert("Update finished, please reboot server.");
				}else if(code == -1){
					alert("update failed, please try again.");
				}else if(code ==-2){
					alert("Download update file failed, please check you internet connection.");
				}else if(code ==-3){
					alert("could not find update.tar.gz in /plant directory!");
				}
		});
		
	});
}

function sysUpdate(opt) {
	var url = $("#updateUrl").val();
	if(opt==1 && url==""){
		alert("Please input update URL.");
		return;
	}
	if (confirm("You are update server system , aren't you?")) {
		disableAllBtn();
		$("#WaitingDialog").dialog('open');
		doSysUpdate(opt);
	}

}

function getsendSetting() {
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "getSendSetting"
	},

	function(data) {
		$(data).find('SendSetting').each(function() {
			$("#resendTimes").val($(this).children('times').text());
			if ($(this).children('broadcast').text() == "1") {
				$("#chk_broadcast").prop("checked", true);
			}
			if ($(this).children('sendConfirm').text() == "1") {
				$("#sendConfirm").prop("checked", true);
			}
			return false;
		});
	});
}


function sendConfirmed() {
	var times = $("#resendTimes").val();
	var broadcat = $("#chk_broadcast").prop("checked") ? 1 : 0;
	var sendConfirm = $("#sendConfirm").prop("checked") ? 1 : 0;
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "SaveSendSetting",
		TIMES: times,
		BROADCAST: broadcat,
		SENDCONFIRM: sendConfirm
	},

	function(data) {
		dealReturnMessage(data, "save Setting");
	});
}

function getSysDate() {
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "GetSysDate"
	}, function(data) {
		setNowDate(data);
	});
}

function backupDB(obj) {
$(obj).attr('disabled',"true");
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "BackupDB"
	}, function(data) {
		//setNowDate(data);
		dealReturnMessage(data, "Backup Database");
		$(obj).removeAttr('disabled');
	});
}

function restoreDB(obj) {
	$(obj).attr('disabled',"true");
var dbName = $("#BackedDBList").val();
//alert(dbName);
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "RestoreDB",
		DBNAME: dbName
	}, function(data) {
		//setNowDate(data);
		dealReturnMessage(data, "Restore Database");
		$(obj).removeAttr('disabled');
	});
}
function getDbList() {
	
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "GetRstoreDB"
	}, function(data) {
		//setNowDate(data);
//alert("OK");
		dealDBList(data);
		
	});
}


function dealDBList(data) {
	var html="";

	$(data).find('backDB').each( function ()
	{
		$("#BackedDBList").append("<option value='"+$(this).children('dbName').text() +"'>"+$(this).children('dbDate').text()+"</option>"); 
	});




}

function setSysDate() {
	var dt = $("#startDatepicker").datepicker("getDate");
	var hour = parseInt($("#dt_hour").val());
	if (isNaN(hour)) {
		alert("The date time is incorrect.");
		return false;
	}
	var minite = parseInt($("#dt_min").val());
	if (isNaN(minite)) {
		alert("The date time is incorrect.");
		return false;
	}
	dt.setHours(hour);
	dt.setMinutes(minite);

	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "setSysDate",
		DATETIME: dt.getTime() / 1000
	}, function(data) {
		dealReturnMessage(data, "Server time setting");
	});
}

function setSysDateByLocal() {
	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "setSysDate",
		DATETIME: (new Date()).getTime() / 1000
	}, function(data) {
		dealReturnMessage(data, "Server time setting");
		getSysDate();
	});
}

function rebootServer() {

	if (!confirm("The server will not response in some minutes during 'reboot',continue?")) {

		return false;
	}

	var loc = "SystemManage.action";
	$.get(loc, {
		METHOD: "RebootServer"
	}, function(data) {});
}

function setNowDate(data) {

	$(data).find('server').each(function() {
		var dateTime = $(this).children('serverDate').text();
		//alert(dateTime);
		var dts = dateTime.split(",");
		var dt = new Date(dts[0], dts[1], dts[2], dts[3], dts[4], dts[5]);

		//alert(dt);
		$("#startDatepicker").datepicker("setDate", dt);
		var hour = dt.getHours();
		if (hour < 10) hour = "0" + hour;
		$("#dt_hour").val(hour);

		var minite = dt.getMinutes();
		if (minite < 10) minite = "0" + minite;
		$("#dt_min").val(minite);
		return false;
	});
	var dt = new Date();
	$("#localTime").html(dt.format("yyyy/MM/dd hh:mm"));
}

function initComp() {
	$("#WaitingDialog").dialog({
		autoOpen: false,
		height: 150,
		width: 180,
		modal: true,
		resizable: false
	});
	$("#startDatepicker").datepicker();
	$("#startDatepicker").datepicker("option", "dateFormat", "yy/mm/dd");
}
