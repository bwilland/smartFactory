
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
	 areaId = $("#nodeArea").val();
	
	 var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "LogQuery",
							STARTTIME : startTime,
							ENDTIME : endTime,
							AREAID : areaId,
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
        METHOD: "LogQuery",
							STARTTIME : startTime,
							ENDTIME : endTime,
							AREAID : areaId,
							PAGENO : pageNo
    },
    function(data) {

        dealLogQuery(data);

    });
}

function logDetail(logId){
	var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "LogDetail",
							LOGID : logId
    },
    function(data) {
        dealLogDetail(data);
    });
}


function dealLogDetail(data){
	var html = "<table width='100%' border='1' cellpadding='0' cellspacing='0' style='text-align:center;'>";

	var pieData = "";
	var ppfd_r,ppfd_b,ppfd_w;

	 $(data).find('log').each(function() {
		ppfd_r = parseFloat($(this).children('PPFD_RED').text());
		ppfd_b = parseFloat($(this).children('PPFD_BLUE').text());
		ppfd_w = parseFloat($(this).children('brightness').text());
		
		html += "<tr><th>Area Name</th><td colspan='2'>"+$(this).children('areaName').text()+"</td></tr>"
		html += "<tr><th>Model</th><td colspan='2'>"+$(this).children('modelName').text()+"</td></tr>"
		html += "<tr><th>Operation</th><td colspan='2'>"+$(this).children('optionType').text()+"</td></tr>";
		html += "<tr><th rowspan='4'>PPFD:</th><td>Tatol(Kμmol/m&sup2;)</td><td>Avg(μmol/m&sup2;/s)</td></tr>"
		html +="<tr style='color:red;'><td>"+ppfd_r+"</td><td>"+$(this).children('red').text()+"</td></tr>";
		html +="<tr style='color:#09C;'><td >"+ppfd_b +"</td><td>"+$(this).children('blue').text()+"</td></tr>";
		html +="<tr><td>"+ppfd_w+"</td><td>"+$(this).children('white').text()+"</td></tr>";
		return false;
	});
	html += "</table>";
	//alert(html);
	$("#logDetailScription").html(html);


	var x = ($(window).width()-400)/2;//使用$(window).width()获得显示器的宽，并算出对应的Div离左边的距离  
	var y = ($(window).height()-600)/2;//使用$(window).height()获得显示器的高，并算出相应的Div离上边的距离 
	$("#logDetail").css("top",y).css("left",x);  
	$("#logDetail").fadeIn();
	//showPie(ppfd_r,ppfd_b,ppfd_w);

//$("#logDetail").dialog( "open" );
}


function showPie(ppfd_r,ppfd_b,ppfd_w){
	plot3 = jQuery.jqplot('chartdiv', 
    [[['PPFD_R', ppfd_r],['PPFD_B', ppfd_b], ['White', ppfd_w]]], 
    {
      title: 'PPFD collation map',
	     seriesColors: [ "red", "#09C", "#F3F3F3"],   
      seriesDefaults: {
        shadow: true, 
        renderer: jQuery.jqplot.PieRenderer, 
        rendererOptions: { 
          sliceMargin: 8, 
          showDataLabels: true 
        } 
      }, 
      legend: { show:true, location: 'e' }
    }
  );

		
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
		html += "<td width='150'>"+$(this).children('areaName').text()+"</td>";
		html += "<td width='120'>"+$(this).children('modelname').text()+"</td>";
		html += "<td width='120'>"+$(this).children('optiontype').text()+"</td>";
		html += "<td width='100'>"+$(this).children('PPF1').text()+"</td>";
		html += "<td width='100'>"+$(this).children('PPF2').text()+"</td>";
		html += "<td width='100'>"+$(this).children('PPF3').text()+"</td>";
		html += "<td width='100'>"+$(this).children('PPF4').text()+"</td></tr>";
	});
	html += "</table>";
	//alert(html);
	$("#LOGLIST").html(html);
}
