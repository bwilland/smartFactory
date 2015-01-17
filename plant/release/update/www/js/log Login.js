
function initComp(){	
	$( "#startDatepicker" ).datepicker();
	$( "#startDatepicker" ).datepicker( "option", "dateFormat","yy/mm/dd" );
	$( "#endDatepicker" ).datepicker();
	$( "#endDatepicker" ).datepicker( "option", "dateFormat", "yy/mm/dd" );
	var dt = new Date();
dt.setMinutes(dt.getMinutes()+5);
$( "#endDatepicker" ).datepicker("setDate",dt);

	var hour = dt.getHours(); 
	if(hour<10)
		hour = "0"+hour;
	var minite = dt.getMinutes(); 
	if(minite<10)
		minite = "0"+minite;
	var timeNow = hour+":" + minite;

	$("#startTime").val(timeNow);
	$("#endTime").val(timeNow);	

		
	dt.setDate(dt.getDate()-1);
	$( "#startDatepicker" ).datepicker("setDate",dt);
	

//$( "#logDetail" ).dialog();
	initPagination();


}

function getUserList() {
    var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "AllUserList"
    },
    function(data) {

        dealSelector(data);

    });
}
function dealSelector(data) {
    var selObj = $("#userList");
    $(data).find('User').each(function() {
        selObj.append("<option value='" + $(this).children('userName').text() + "'>" + $(this).children('userName').text() + "</option>");
    });
}

function searchLog(){
	
		var sDate = $("#startDatepicker").datepicker( "getDate" );
		var sTime = $("#startTime").val().split(":");
		if(sTime[0].length<1 || sTime[1].length<1){
			alert("Start Time incorrect.");
			return false;
			}
			sDate.setHours(sTime[0]);
			sDate.setMinutes(sTime[1]);
		 startTime = sDate.getTime()/1000;
		 sDate = $("#endDatepicker").datepicker( "getDate" );
	 		sTime = $("#endTime").val().split(":");
	if(sTime[0].length<1 || sTime[1].length<1){
		alert("End Time incorrect.。");
		return false;
		}
		sDate.setHours(sTime[0]);
		sDate.setMinutes(sTime[1]);
		endTime = sDate.getTime()/1000;
	if(endTime < startTime){
			alert("Start time must be earlier than end time.")	
			return false;
	}
	 userId = $("#userList").val();
	
	 var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "LogQuery",
							STARTTIME : startTime,
							ENDTIME : endTime,
							USERID : userId,
							PAGENO : 1
    },
    function(data) {
							dealPage(data);
        dealLogQuery(data);

    });
}

function logQuery(pageNo){
	var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "LogLoginQuery",
							STARTTIME : startTime,
							ENDTIME : endTime,
							userId : userId,
							PAGENO : pageNo
    },
    function(data) {

        dealLogQuery(data);

    });
}




function initPagination() {
    $('#JQpagination').jqPagination({
        page_string:'Page {current_page} of {max_page} ',
        paged: function(page) {
            logQuery(page);
        }
    });
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

function dealLogQuery(data){
	var html = "<table width='100%' border='0' cellspacing='0' class='tableStyle'>";
	var logTime,dt;
	dt = new Date();
	 $(data).find('log').each(function() {
		logTime = parseInt($(this).children('datetime').text())*1000;
		dt.setTime(logTime);

		//html += "<tr class='trStyle' onclick='logDetail("+$(this).children('id').text()+");'><td width='140'>"+dt.format("yyyy/MM/dd hh:mm")+"</td>";
		html += "<tr ><td width='140'>"+dt.format("yyyy/MM/dd hh:mm")+"</td>";
		html += "<td width='150'>"+$(this).children('userName').text()+"</td>";
		html += "<td width='120'>"+$(this).children('ip').text()+"</td>";
		html += "<td width='120'>"+$(this).children('result').text()+"</td></tr>";
		
	});
	html += "</table>";
	//alert(html);
	$("#LOGLIST").html(html);
}
