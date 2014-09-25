
$(document).ready(function(){
	//players
	$('#btnPlayerListRefresh').click(function(){
		playerList();
	});
	$('#btnPlayerCreate').click(function(){
		playerCreate($('#txtPlayerCreateId').val(), $('#txtPlayerCreateUsername').val())
		$('#txtPlayerCreateId').val('');
		$('#txtPlayerCreateUsername').val('');
	});	
	
	//attacks
	$('#btnAttackListRefresh').click(function(){
		attackList();
	});
	$('#btnAttackCreate').click(function(){
		attackCreate($('#txtAttackCreateId').val(), $('#txtAttackCreateName').val())
		$('#txtAttackCreateId').val('');
		$('#txtAttackCreateName').val('');
	});	
	
	//games
	$('#btnGameListRefresh').click(function(){
		gameList();
	});
	$('#btnGameCreate').click(function(){
		gameCreate($('#selGameCreateVictimId').val(), $('#selGameCreateAttackerId').val(), $('#selGameCreateGame').val());
		$('#selGameCreateVictimId').selectedIndex = 0;
		$('#selGameCreateAttackerId').selectedIndex = 0;
		$('selGameCreateGame').selectedIndex = 0;
	});	
	
	attackList();	
	playerList();
	gameList();
}); 


function playerList() {
	$.getJSON('/vandalizr.io/?method=playerList', function(data){
		$('#pnlPlayerList').empty();
		$.each(data, function(i, obj) {
			if(obj.id) {
				$('#pnlPlayerList').append('<div class="input-group"><span class="input-group-addon">' + obj.id + '</span><input type="text" class="form-control" readonly="readonly" value="' + obj.name + '"><span class="input-group-btn"><button class="btn btn-danger btnRemove" type="button" id="remove' + obj.id + '">Delete <span class="glyphicon glyphicon-remove-sign"></span></button></span></div><br />');
			}
		});
		initPlayerEvents();
	});
}
function playerCreate(id, username) {
	$.getJSON('/vandalizr.io/?method=playerCreate&id=' + id + '&username=' + username, function(data){
		playerList();
	});
}
function playerDelete(id) {
	$.getJSON('/vandalizr.io/?method=playerDelete&id=' + id, function(data){
		playerList();
	});
}
function initPlayerEvents() {
	$('.btnRemove').click(function(){
        var id = this.id.substr(6);
		playerDelete(id);
    });

}


function attackList() {
	$.getJSON('/vandalizr.io/?method=attackList', function(data){
		$('#pnlAttackList').empty();
		$.each(data, function(i, obj) {
		  if(obj.id) {
			$('#pnlAttackList').append('<div class="input-group"><span class="input-group-addon">' + obj.id + '</span><input type="text" class="form-control" readonly="readonly" value="' + obj.name + '"><span class="input-group-btn"><button class="btn btn-danger btnRemoveAttack" type="button" id="remove' + obj.id + '">Delete <span class="glyphicon glyphicon-remove-sign"></span></button></span></div><br />');
		  }
		});
		initAttackEvents();
	});
}
function attackCreate(id, name) {
	$.getJSON('/vandalizr.io/?method=attackCreate&id=' + id + '&name=' + name, function(data){
		attackList();
	});
}
function attackDelete(id) {
	$.getJSON('/vandalizr.io/?method=attackDelete&id=' + id, function(data){
		attackList();
	});
}
function initAttackEvents() {
	$('.btnRemoveAttack').click(function(){
        var id = this.id.substr(6);
		attackDelete(id);
    });

}

function gameList() {	
	$('#selGameCreateVictimId').empty();
	$('#selGameCreateAttackerId').empty();
	$.getJSON('/vandalizr.io/?method=playerList', function(data){
		$.each(data, function(i, obj) {
		  if(obj.id) {
			$('#selGameCreateVictimId').append($('<option>').val(obj.id).text(obj.name));
			$('#selGameCreateAttackerId').append($('<option>').val(obj.id).text(obj.name));
		  }
		});
	});	

	$.getJSON('/vandalizr.io/?method=gameList', function(data){
		$('#pnlGameList').empty();
		$.each(data, function(i, obj) {
		  if(obj.id) {
			$('#pnlGameList').append('<div class="input-group"><span class="input-group-addon">' + obj.id + '</span><input type="text" class="form-control" readonly="readonly" value="' + obj.attacker + ' vs. ' + obj.victim + '"><span class="input-group-btn"><button class="btn btn-info btnViewGame" type="button" id="view' + obj.id + '">View <span class="glyphicon glyphicon-info-sign"></span></button><button class="btn btn-danger btnRemoveGame" type="button" id="remove' + obj.id + '">Delete <span class="glyphicon glyphicon-remove-sign"></span></button></span></div><br />');
		  }
		});
		initGameEvents();
	});
}
function gameCreate(victim, attacker, game) {
	$.getJSON('/vandalizr.io/?method=gameCreate&victim=' + victim + '&attacker=' + attacker + '&game=' + game, function(data){
		gameList();
	});
}
function gameDelete(id) {
	$.getJSON('/vandalizr.io/?method=gameDelete&id=' + id, function(data){
		gameList();
	});
}
function initGameEvents() {
	$('.btnRemoveGame').click(function(){
        var id = this.id.substr(6);
		gameDelete(id);
    });

}