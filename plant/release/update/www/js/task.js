function getTaskId() {
	var url = window.location.href;

	var paras = url.split("?");
	var taskID = -1;
	if (paras[1] != undefined) {
		CurrentTaskID = parseInt(paras[1]);
	}
}

function initComp() {
	$("#startDatepicker").datepicker();
	$("#startDatepicker").datepicker("option", "dateFormat", "yy/mm/dd");
	$("#endDatepicker").datepicker();
	$("#endDatepicker").datepicker("option", "dateFormat", "yy/mm/dd");
	var now = new Date();
	$("#startDatepicker").datepicker("setDate", now);
	$("#startTime").val("05:00");
	var sevendayslater = new Date(now.valueOf()+7*24*3600000);
	$("#endDatepicker").datepicker("setDate", sevendayslater);
	$("#endTime").val("20:00");
}


function getAreaList() {
	var loc = "AreaManage.action";
	$.get(loc, {
		METHOD: "AreaList"
	}, function(data) {
		dealAreaList(data);
	});
}

function dealAreaList(data) {
	var html = "";
	$(data).find('domain').each(function() {
		var areaId = $(this).children('id').text();
		html += "<div class='item' onclick='selectArea(" + areaId + ");'> <input id='isAreaSelected" + areaId + "' name='isAreaSelected' type='checkbox' onclick='preventEvent(event);' value='" + areaId + "' />";

		html += "<label class='nodes' id='AreaName" + areaId + "'>" + $(this).children('name').text() + "</label><label class='node-count'>" + $(this).children('nodecount').text() + "</label></div>";

	});
	$("#AREALIST").html(html);


}


function getModelList() {
	var loc = "ModelManage.action";
	$.get(loc, {
		METHOD: "ModelInfoList"
	}, function(data) {
		dealModelList(data);
	});
}

function dealModelList(data) {
	var html = "<table class='tableStyle' width='100%' cellpadding='0' cellspacing='0'> ";
	$(data).find('model').each(function() {
		html += "<tr class='trStyle'><td width='100' >" + $(this).children('name').text() + "</td>";
		html += "<td width='60' >" + $(this).children('PPF1').text() + "</td>";
		html += "<td width='60' >" + $(this).children('PPF2').text() + "</td>";
		html += "<td width='60' >" + $(this).children('PPF3').text() + "</td>";
		html += "<td width='60' >" + $(this).children('PPF4').text() + "</td>";
		html += "<td width='35' ><div class='modelSelect bgColorgray' onClick='selectModel(" + $(this).children('id').text() + ",\"" + $(this).children('name').text() + "\")'>&raquo;</div></td></tr>";

	});
	html += "</table>";

	$("#MODELLIST").html(html);


}


//编辑定时任务 用

function getTaskInfo() {

	var loc = "ScheduleManage.action";
	$.get(loc, {
		METHOD: "ScheduleInfo",
		SCHEDULEID: CurrentTaskID
	}, function(data) {
		dealTaskInfo(data);
		dealSelectedModelList(data);
	});
}

function dealTaskInfo(data) {
	$(data).find('schedule').each(function() {
		var startTime = $(this).children('starttime').text();
		var endTime = $(this).children('endtime').text();

		$("#taskDescript").val($(this).children('description').text());
		var areaList = $(this).children('domain').text();
		var dt = new Date();
		dt.setTime(parseInt(startTime) * 1000);
		$("#startDatepicker").datepicker("setDate", dt);
		var hour = dt.getHours();
		if (hour < 10) hour = "0" + hour;

		var minite = dt.getMinutes();
		if (minite < 10) minite = "0" + minite;
		var timeNow = hour + ":" + minite;
		$("#startTime").val(timeNow);

		dt.setTime(parseInt(endTime) * 1000);
		$("#endDatepicker").datepicker("setDate", dt);

		hour = dt.getHours();
		if (hour < 10) hour = "0" + hour;

		minite = dt.getMinutes();
		if (minite < 10) minite = "0" + minite;
		timeNow = hour + ":" + minite;
		$("#endTime").val(timeNow);

		var areaIds = areaList.split(",");
		var i = 0;
		for (i = 0; i < areaIds.length; i++) {
			$("#isAreaSelected" + areaIds[i]).prop("checked", true);
		}

		var timeSpan = parseInt(endTime) - parseInt(startTime);
		timeSpan = Math.round(timeSpan / 360);
		$("#duringTime").val(timeSpan / 10);

	});
}

function dealSelectedModelList(data) {
	var html = "<table class='tableStyle' id='distModelList' cellpadding='0' cellspacing='0' width='100%'> ";
	$(data).find('task').each(function() {
		var during = $(this).children('during').text();
		html += "<tr class='trStyle'><td width='33'><img src='./img/down.png'/></td>";
		html += "<td width='70' >" + $(this).children('name').text() + "</td>";
		html += "<td width='80' > <input type='text'  value='" + $(this).children('during').text() + "' class='textbox' style='width:40px;'  onblur='inputDuring(" + distModelCount + ",this);'/></td>";
		html += "<td width='33' ><div class='modelDelete' onclick='removeRow(" + distModelCount + ",this);'>X</div></td></tr>";
		//json数据
		arraySelectedMode.push(newModelData(distModelCount, $(this).children('modelid').text(), during));
		distModelCount++;
	});
	html += "</table>";
	$("#DISTMODELLIST").html(html);


}

function setTimeSpan() {

	var sDate = $("#startDatepicker").datepicker("getDate");
	var sTime = $("#startTime").val().split(":");
	if (sTime[0].length < 1 || sTime[1].length < 1) {
		return 0;
	}
	sDate.setHours(sTime[0]);
	sDate.setMinutes(sTime[1]);

	var startTime = sDate.getTime() / 1000 / 60;

	sDate = $("#endDatepicker").datepicker("getDate");
	sTime = $("#endTime").val().split(":");
	if (sTime[0].length < 1 || sTime[1].length < 1) {
		return 0;
	}
	sDate.setHours(sTime[0]);
	sDate.setMinutes(sTime[1]);
	var endTime = sDate.getTime() / 1000 / 60;
	var result = endTime - startTime;
	if (result < 0) {
		alert("Start Time must be early than end time.");
		$("#duringTime").val(0);
		return false;
	} else {

		result = Math.round(result / 6);

		$("#duringTime").val(result / 10); //一位小数
	}
	//return result;
}

function selectModel(modelId, modelName) {
	var startHour = 0;
	var sTime = $("#startTime").val().split(":");
	if (sTime[0].length > 0 && sTime[1].length >0) {
		startHour = sTime[0];
	}

	var timeDuring = 0;
	var i = 0;
	for (i = 0; i < arraySelectedMode.length; i++) {
		timeDuring += arraySelectedMode[i].during;
	}

	timeDuring = 24 -startHour- timeDuring;
	if (timeDuring < 0) {
		timeDuring = 0;
	}

	var tableModel = $("#distModelList");
	var newRow = $("<tr class='trStyle'></tr>");
	var nTd = $("<td width='33'><img src='./img/down.png'/></td>");
	newRow.append(nTd);
	nTd = $("<td width='70'>" + modelName + "</td>");
	newRow.append(nTd);

	nTd = $("<td width='80'></td>");
	nTd.append($("<input type='text'  value='" + timeDuring + "' class='textbox' style='width:40px;' onblur='inputDuring(" + distModelCount + ",this);'/>"));
	newRow.append(nTd);

	nTd = $("<td width='33'></td>");
	nTd.append($("<div class='modelDelete' onclick='removeRow(" + distModelCount + ",this);'>X</div>"));
	newRow.append(nTd);
	tableModel.append(newRow);


	//json数据
	arraySelectedMode.push(newModelData(distModelCount, modelId, timeDuring));
	distModelCount++;
}


//修改持续时间

function inputDuring(rowIndex, obj) {
	if (isNaN(parseFloat($(obj).val()))) {
		alert("During time is incorrect.");
		$(obj).val("0");
		return false;
	}
	for (var i = 0; i < arraySelectedMode.length; i++) {
		if (arraySelectedMode[i].id == rowIndex) {
			arraySelectedMode[i].during = parseFloat($(obj).val());
			break;
		}
	}
}

function removeRow(rowIndex, obj) {
	$(obj).parent().parent().remove();

	//删除json数据
	for (var i = 0; i < arraySelectedMode.length; i++) {
		if (arraySelectedMode[i].id == rowIndex) {
			arraySelectedMode.splice(i, 1);
			break;
		}
	}

}

//-------------------------------------------------------

//新建模式的json数据

function newModelData(rowID, modelId, during) {
	var strJson = '{"id":' + rowID + ',"modelId":' + modelId + ',"during":' + during + '}';
	var jsonObj = $.parseJSON(strJson);
	return jsonObj;
}

//----------------------------------------------------------

function finished(objButton) {

	$(objButton).attr('disabled', "true");

	var i = 0;
	var areas = "";
	var areaNames ="";
	var modelID = "";
	var descript = $("#taskDescript").val();

	var sDate = $("#startDatepicker").datepicker("getDate");
	var sTime = $("#startTime").val().split(":");
	if (sTime[0].length < 1 || sTime[1].length < 1) {
		alert("Start time is incorrect.");
		$(objButton).removeAttr("disabled");
		return false;
	}
	sDate.setHours(sTime[0]);
	sDate.setMinutes(sTime[1]);

	var startTime = sDate.getTime() / 1000;

	var eDate = $("#endDatepicker").datepicker("getDate");
	sTime = $("#endTime").val().split(":");
	if (sTime[0].length < 1 || sTime[1].length < 1) {
		alert("End time is incorrect.");
		$(objButton).removeAttr("disabled");
		return false;
	}
	eDate.setHours(sTime[0]);
	eDate.setMinutes(sTime[1]);
	var endTime = eDate.getTime() / 1000;
	if (endTime < startTime) {
		alert("Start Time must be early than end time.");
		$(objButton).removeAttr("disabled");
		return false;
	}
	
		var dt = new Date();
		if (endTime < (dt.getTime()/1000)) {
		alert("End Time is early than NOW,please correct.");
		$(objButton).removeAttr("disabled");
		return false;
	}
		

	$("[name='isAreaSelected']").each(function() {
		if ($(this).prop("checked")) {
			areas += $(this).val() + ",";
			areaNames += $("#AreaName"+$(this).val()).html();
			areaNames += ";";
		}
	});
	if (areas == "") {
		alert("Area please.");
		$(objButton).removeAttr("disabled");
		return false;
	}

	var tatolTimeSpan = 0;
	if (arraySelectedMode.length == 0) {
		alert("Model please.");
		$(objButton).removeAttr("disabled");
		return false;
	} else {

		for (i = 0; i < arraySelectedMode.length; i++) {
			if (arraySelectedMode[i].during == 0) {
				alert("please input during time for NO.[" + arraySelectedMode[i].id + "].");
				$(objButton).removeAttr("disabled");
				return false;
			}
			tatolTimeSpan += arraySelectedMode[i].during;
		}
	}


	var startHour = sDate.getHours();

	if (tatolTimeSpan > 24-startHour) {
		alert("Task's during time more than schedule during Time, please correct. ");
		$(objButton).removeAttr("disabled");
		return false;
		

	}

	//整理序号，使之顺序排列
	for (i = 0; i < arraySelectedMode.length; i++) {
		arraySelectedMode[i].id = i + 1;
	}
	var modelList = JSON.stringify(arraySelectedMode);
	//alert(modelList);

	var loc = "ScheduleManage.action";
	$.get(loc, {
		METHOD: "SaveSchedule",
		SCHEDULEID: CurrentTaskID,
		STARTTIME: startTime,
		ENDTIME: endTime,
		DESCRIPTION: areaNames,
		AREAID: areas,
		MAXSQUENCE: arraySelectedMode.length,
		MODELLIST: modelList

	}, function(data) {
		dealSaveMessage(data)
	});
}

function dealSaveMessage(data) {
	$(data).find('return').each(function() {
		if (parseInt($(this).children('code').text()) == 2) {
			alert("Schdule saving,please wait a monent...");
		}
		self.location.href = "schedule.html";
	});
}

function changeDate(obj) {
	var value = $(obj).val();
	if (isNaN(value)) {
		alert("Please input number.");
		return false;
	}
	var sDate = $("#startDatepicker").datepicker("getDate");
	var sTime = $("#startTime").val().split(":");
	sDate.setHours(sTime[0]);
	sDate.setMinutes(sTime[1]);

	var minitesValue = parseFloat(value) * 60;
	//alert(minitesValue);

	sDate.setMinutes(sDate.getMinutes() + minitesValue);

	$("#endDatepicker").datepicker("setDate", sDate);
	var hour = sDate.getHours();
	if (hour < 10) hour = "0" + hour;
	var minite = sDate.getMinutes();
	if (minite < 10) minite = "0" + minite;
	var timeNow = hour + ":" + minite;
	$("#endTime").val(timeNow)
}


function selectAllArea(obj) {
	var isSelected = $(obj).prop("checked");
	$("[name='isAreaSelected']").each(function() {
		$(this).prop("checked", isSelected);
	});
}
