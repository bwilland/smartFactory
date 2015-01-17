

function getScheduleList(){
	//$("#DEVICELISTER").html('请稍候...');
	var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "ScheduleList",
		CLAUSE : "0,1,2,4",
		PAGENO : 1
	},function(data) {dealShowScheduleList(data);});
}

function dealShowScheduleList(data){
	
	dealPage(data);
	
	var dt = new Date();
	var startTime,endTime,modiTime;
	var html="<table width='100%'  cellpadding='0' cellspacing='0' class='tableStyle'>";
	$(data).find('schedule').each( function ()  
	{ 
		
		html+="<tr class='trStyle' onclick='selectTask("+$(this).children('id').text()+","+$(this).children('statusId').text() +",this);'>"
		html+="<td width='200' align='center'>"+$(this).children('domain').text()+"</td>";
		html+="<td width='80' align='center'>"+$(this).children('status').text()+"</td><td width='110'>";
		 startTime = parseInt($(this).children('startTime').text())*1000;
		 endTime = parseInt($(this).children('endTime').text())*1000;
		 modiTime = parseInt($(this).children('modifyTime').text())*1000;
		dt.setTime(startTime);
		html+=dt.format("yyyy/MM/dd hh:mm")+"</td><td width='110'>";
		dt.setTime(endTime);
		html+=dt.format("yyyy/MM/dd hh:mm")+"</td><td width='220'>";
		html+=$(this).children('description').text()+"</td><td width='110'>";
		dt.setTime(modiTime)
		html+=dt.format("yyyy/MM/dd hh:mm")+"</td><td width='50'>"
		html+="<input type='button' onclick='scheduleDetail("+$(this).children('id').text()+");' value='detail'/></td></tr>";
	
	});

 	html+="</table>";
 	
	$("#SCHEDULELIST").html(html);
}

function selectTask(taskId,taskStatus,obj){
	if(selectedTaskId != -1){
		$(obj_selectTaskRow).removeClass("trChecked").addClass("trStyle");
	}
	$(obj).removeClass("trStyle").addClass("trChecked");
	selectedTaskId = taskId;
	selectedTaskStatus = taskStatus;
	obj_selectTaskRow = obj;

	//scheduleDetail(selectedTaskId);
	
}

function searchTask(pageNo){
	var clause = "";
	$("[name='rdQuery']").each(function(){
				if($(this).prop("checked")){
					clause += $(this).val()+",";
				}
	});
	clause += "4";
	var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "ScheduleList",
		CLAUSE : clause,
		PAGENO : pageNo
	},function(data) {dealShowScheduleList(data);});
}

function dealPage(data){
		 $(data).find('page').each(function() {
        var pageNo = parseInt($(this).children('pageno').text());
        var maxPage = parseInt($(this).children('maxpage').text());
        //initPagination(pageNo,maxPage);
        $('#JQpagination').jqPagination('option', {current_page:pageNo,max_page:maxPage},true);

        
        return false;
    });
	}
//---------------------------------------------------------
function editTask(){
	//selectedTaskId
	if(selectedTaskId ==-1){
		alert("Please select one schedule.")	;
			return false;
		}
	if(selectedTaskStatus==1){
			alert("Schedule is running, can not be edited.")	;
			return false;
		}
	self.location.href='task.html?'+selectedTaskId;
}

function deleteTask(){
	//selectedTaskId
	if(selectedTaskId ==-1){
		alert("Please select one schedule at least.");
			return false;
		}
		if(selectedTaskStatus!=3){
			alert("The schedule is not end, can not be deleted.")	;
			return false;
		}
		if(!confirm("You are deleting schedule, are you?")){
			return false;
			}
	var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "DeleteSchedule",
		SCHEDULEID : selectedTaskId
	},function(data) {if(dealReturnMessage(data,"Delete")>0){searchTask();}});
}
function pauseTask(){
	//selectedTaskId
if(selectedTaskId ==-1){
		alert("Please select one schedule at least.");
			return false;
		}
		if(selectedTaskStatus != 1){
			alert("The Schedule is paused or not running.");
			return false;
		}
				disableAllBtn();
	var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "ModifySchedule",
		SCHEDULEID : selectedTaskId,
		OPTION : 0
	},function(data) {if(dealReturnMessage(data,"Pause/Watch")>0){enableAllBtn(); searchTask();}});
}
function resumeTask(){
	//selectedTaskId
	
	if(selectedTaskId ==-1){
		alert("Please select one schedule at least.");
			return false;
		}
		if(selectedTaskStatus != 2 && selectedTaskStatus!= 1){
			alert("The Schedule is not paused or not running.");
			return false;
		}
		disableAllBtn();
	var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "ModifySchedule",
		SCHEDULEID : selectedTaskId,
		OPTION : 1
	},function(data) {if(dealReturnMessage(data,"Resume")>0){enableAllBtn(); searchTask();}});
}
function stopTask(){
	//selectedTaskId
	if(selectedTaskId ==-1){
		alert("Please select one schedule at least.");
			return false;
		}
		if(selectedTaskStatus ==3){
			alert("The Schedule is end already.");
			return false;
		}
		disableAllBtn();
var loc = "ScheduleManage.action";
	$.get(loc,{
		METHOD : "ModifySchedule",
		SCHEDULEID : selectedTaskId,
		OPTION : 2
	},function(data) {if(dealReturnMessage(data)>0){enableAllBtn(); searchTask();}});
	
}

function scheduleDetail(scheduleId){
	var loc = "ScheduleManage.action";
    $.get(loc, {
        METHOD: "ScheduleDetail",
	SCHEDULEID : scheduleId
    },
    function(data) {
        dealscheduleDetail(data);
    });
}


function dealscheduleDetail(data){
	var html = "<table width='100%' border='1' cellpadding='0' cellspacing='0' style='text-align:center;'>";
	var currentSquence;
	 $(data).find('schedule').each(function() {
		currentSquence = $(this).children('currentSequence').text();
		html += "<tr><th width='80'>Description</th><td>"+$(this).children('description').text()+"</td></tr>"

		html += "<tr><th>Status</th><td>"+$(this).children('status').text()+"</td></tr>"
		html += "<tr><th>Area</th><td>"+$(this).children('domain').text()+"</td></tr>"
		
		return false;
	});
	html += "</table>";

	$("#scheduleDetailScription").html(html);

	var taskDetail = "<table width='100%' border='1' cellpadding='0' cellspacing='0' style='text-align:center;'>"
	+"<tr bgcolor='#86c111'>	<th width='40'>NO. </th>	<th>Model </th>	<th width='120'>Duration<br/>(hrs) </th>	</tr>";
	
 $(data).find('task').each(function() {
		 	var squence = $(this).children('sequence').text();
		 	var selected = "";
		 	if(currentSquence===squence){
		 			taskDetail +="<tr class='selected'>";
		 		}else{
		 			taskDetail +="<tr>";
		 	}
 		taskDetail +="<td>"+squence+"</td><td>"+$(this).children('name').text()+"</td><td>"+$(this).children('during').text()+"</td></tr>";
 	});
	taskDetail += "</table>";
	
	//	alert(taskDetail);
	$("#taskDiv").html(taskDetail);

	//var x = ($(window).width()-500)/2;//使用$(window).width()获得显示器的宽，并算出对应的Div离左边的距离  
	//var y = ($(window).height()-600)/2;//使用$(window).height()获得显示器的高，并算出相应的Div离上边的距离 
	//$("#scheduleDetail").css("top",y).css("left",x);  
	//$("#scheduleDetail").fadeIn();
	$("#scheduleDetail").dialog("open");

}

function initPagination() {
    $('#JQpagination').jqPagination({
        page_string:'Page {current_page} of {max_page} ',
        paged: function(page) {
            searchTask(page);
        }
    });
}
