
function homeList(pageNo){
	var loc = "HomeManage.action";
	$.get(loc,{
		METHOD : "HomeAll",
		PAGENO : pageNo
	},function(data) {dealPage(data);dealHomeList(data);});
}

function homeListPage(pageNo){
	var loc = "HomeManage.action";
	$.get(loc,{
		METHOD : "HomeAll",
		PAGENO : pageNo
	},function(data) {dealHomeList(data);});
}

function dealPage(data){
		 $(data).find('page').each(function() {
        var pageNo = parseInt($(this).children('pageno').text());
        var maxPage = parseInt($(this).children('maxpage').text());
        initPagination(maxPage);
        return false;
    });
	}

function dealHomeList(data) {
	
	var html ="";
var red = 0,blue = 0,brightness = 0;
	$(data).find('domain').each( function ()
	{ 

		html += "<div class='areainfo'><center><h3>"+ $(this).children('name').text() +"</h3></center>";
		var onoffCss = $(this).children('onoffST').text()=="0"?"areaOff":"";
		html += "<table class='table table-bordered'><tr><td width='90'>Status：</td><td ><div class='areaOn "+onoffCss +"'></div></td></tr>";
		html += "<tr><td>Operation：</td><td width='197'>"+$(this).children('optionType').text()+"</td></tr>";
		html += "<tr><td>Model：</td><td>"+$(this).children('modelName').text()+"</td></tr>"
		
	
		
		html += "<tr><td colspan='2' height='80px'><table width='100%' height='100%' border='0' cellspacing='0' cellpadding='0'><tr>";
		ppf1 = parseInt($(this).children('ppf1').text());
		html += "<td width='20%'>PPF:</td><td width='22%' valign='bottom'><div class='ppfd_border'><div id='PPFD_R' class='ppdf-r' style='height:"+ppf1+"px;margin-top:"+(100-ppf1)+"px;'></div></div>"+$(this).children('ppf1').text() +"</td>";
		ppf2 = parseInt($(this).children('ppf2').text());
		html += "<td width='20%' valign='bottom'><div class='ppfd_border'><div id='PPFD_B' class='ppdf-b' style='height:"+ppf2+"px;margin-top:"+(100-ppf2)+"px;'></div></div>"+$(this).children('ppf2').text() +"</td>";
		ppf3 = parseInt($(this).children('ppf2').text());
			html += "<td width='20%' valign='bottom'><div class='ppfd_border'><div id='PPFD_N' class='ppdf-n' style='height:"+ppf3+"px;margin-top:"+(100-ppf3)+"px;'></div></div>"+$(this).children('ppf3').text() +"</td>";
		ppf4 = parseInt($(this).children('ppf3').text());
		html += "<td width='20%' valign='bottom'><div class='ppfd_border'><div id='brightness' class='brightness' style='height:"+ppf4+"px;margin-top:"+(100-ppf4)+"px;'></div></div>"+ppf4+"</td>";
		html += "</tr></table></td></tr></table></div>";

	});

	//alert(html);
	$("#AREALIST").html(html);
	
}

function initPagination(maxPage) {
    $('#JQpagination').jqPagination({
    				max_page : maxPage,
    				current_page :1,
        page_string:'Page {current_page}   of {max_page} ',
        paged: function(page) {
        				current_pageNo = page;
            homeListPage(page);
        }
    });
}

//----------------------------refresh------------------------------------------
function refreshArea(){
	
	homeListPage(current_pageNo);
}

function startRefreshInv()
{
	interval = setInterval(refreshArea, "9000");
}

function stopRefreshInv()
{
	clearTimeout(interval);
}


