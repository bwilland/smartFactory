//-------------User -----------------------
function getUserList(pageNo) {
    var loc = "HomeManage.action";
    $.get(loc, {
        METHOD : "AllUserList"
    },
    function(data) {
    	//dealPage(data);
        dealUserList(data);
    });
}
	
	
function dealUserList(data) {
    var html = "";
    
    $(data).find('User').each(function() {
	var id = $(this).children('userId').text();
       
        html += "<div class='item' onclick='showUserInfo(" + id + ");'>";
        html += "<label class='Users'>"+$(this).children('userName').text();
        html += "</label></div>";

    }); 
   //alert(html);
    $("#USERLIST").html(html);
    

}






function showUserInfo(UserId) {

    var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "UserInfo",
        USERID: UserId
    },
    function(data) {
        dealUserInfo(data);
    });
}

function dealUserInfo(data) {
    $(data).find('User').each(function() {
        $("#userId").val($(this).children('UserId').text());
        $("#userName").val($(this).children('UserName').text());
	$("#userPwd").val($(this).children('UserPwd').text());
        $("#role").val($(this).children('Role').text());
       $("#userName").attr("disabled",true);
        return false;
    });

}

function newUser() {
    $("#userId").val("-1");
    $("#userName").val("");
    $("#role").val(0);
    $("#userPwd").val("");
$("#userName").attr("disabled",false);
}

function saveUser(objButton) {
	$(objButton).attr('disabled',"true");
    var UserID = $("#userId").val();
    var userPwd = $("#userPwd").val();
    var role = $("#role").val();
    var userName = $("#userName").val();

    if (userName == "") {
        alert("UserName please .");
        $("#userName").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }
    if (userPwd.length ==0) {
        alert("userPwd is required.");
        $("#userPwd").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }

    

    saveUserDB(UserID, userName, userPwd, role,objButton);
    
}
function saveUserDB(UserID, userName, userPwd, role,objButton) {
    var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "SaveUser",
        UserID: UserID,
        USERNAME: userName,
        USERPWD: userPwd,
        ROLE: role
    },
    function(data) {
    	$(objButton).removeAttr("disabled");	
    	var returnCode = dealReturnMessage_eM(data, "Save User ","The Users has been saved already.");
    	if(returnCode>0){
        $("#userId").val(returnCode);
        getUserList();
     }
        

    });
}
function deleteUser() {
    var UserID = $("#userId").val();
  var userName = $("#userName").val();
	if(userName=="admin"){
		alert("User [admin] can't be deleted.");
		return false;
	}
	if(!confirm("You are deleting User,aren't you?")){
		return false;
	}
	
    var loc = "HomeManage.action";
    $.get(loc, {
        METHOD: "DeleteUser",
        USERID: UserID
    },
    function(data) {
        if (dealReturnMessage(data, "Delete Users") > 0) {
            newUser();
            getUserList();
        }
    });
}



	

	

