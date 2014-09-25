<?php
function __autoload($class_name) {
    include $class_name . '.php';
}

const DATA_PATH = 'D:/Domains/mirz.com/wwwroot/Vandalizr.io/data';

$method = $_GET['method'];

if(!$method) {
	die();
}

use Flintstone\Flintstone;

try {
    // "databases"
	$db_audit = Flintstone::load('audit', array('dir' => DATA_PATH));
	$db_players = Flintstone::load('players', array('dir' => DATA_PATH));
    $db_games = Flintstone::load('games', array('dir' => DATA_PATH));
	$db_attacks = Flintstone::load('attacks', array('dir' => DATA_PATH));
}
catch (FlintstoneException $e) {
    die();
}

function audit($db, $id, $label, $data) {
	$db->set(time().'-'.$id, array('action' => $label, 'data' => $data));
}

switch($method) {
	case 'playerList':
		$keys = $db_players->getKeys();
		$data = array();
		$index = 0;
		foreach ($keys as $id) {
			$user = $db_players->get($id);
			$player = array();
			$player['id'] = $id;
			$player['name'] = $user['username'];
			$data[$index] = $player;
			$index++;
		}
		if($index==0) {
			$data[0] = 0;
		}
		break;
	case 'playerCreate':
		$id = $_GET['id'];
		$username = $_GET['username'];
		if($id) {
			if(!$username) {
				$index = 0;
				$keys = $db_players->getKeys();
				foreach ($keys as $id) {
					$index++;
				}
				$username = 'Player'.($index+1);
			}
			$data = array('username' => $username, 'connected' => date('Y/m/d H:i:s'));
			$db_players->set($id, $data);
			audit($db_audit, $id, $method, $data);
			
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		break;
	case 'playerDelete':
		$id = $_GET['id'];
		if($id) {
			$db_players->delete($id);
			audit($db_audit, $id, $method, array('disconnected' => date('Y/m/d H:i:s')));
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		//TODO:
		//remove any of their games etc
		//maybe have a history database for wins/losses etc
		break;	
	case 'attackList':
		$keys = $db_attacks->getKeys();
		$data = array();
		$index = 0;
		foreach ($keys as $id) {
			$item = $db_attacks->get($id);
			$attack = array();
			$attack['id'] = $id;
			$attack['name'] = $item['name'];
			$data[$index] = $attack;
			$index++;
		}
		if($index==0) {
			$data[0] = 0;
		}
		break;
	case 'attackCreate':
		$id = $_GET['id'];
		$name = $_GET['name'];
		if($id && $name) {
			$data = array('name' => $name, 'added' => date('Y/m/d H:i:s'));
			$db_attacks->set($id, $data);
			audit($db_audit, $id, $method, $data);
			
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		break;
	case 'attackDelete':
		$id = $_GET['id'];
		if($id) {
			$db_attacks->delete($id);
			audit($db_audit, $id, $method, array('deleted' => date('Y/m/d H:i:s')));
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		break;
	case 'gameList':
		$keys = $db_games->getKeys();
		$data = array();
		$index = 0;
		foreach ($keys as $id) {
			$game = $db_games->get($id);
			$instance = array();
			$instance['id'] = $id;
			$instance['victim'] = $game['victim'];
			$instance['attacker'] = $game['attacker'];
			$instance['game'] = $game['game'];
			$instance['challenged'] = $game['challenged'];
			$data[$index] = $instance;
			$index++;
		}
		if($index==0) {
			$data[0] = 0;
		}
		break;
	case 'gameCreate':
		$id = uniqid();
		$victim = $_GET['victim'];
		$attacker = $_GET['attacker'];
		$game = $_GET['game'];
		if($id && $victim && $attacker) {
			$data = array('victim' => $victim, 'attacker' => $attacker, 'game' => $game, 'challenged' => date('Y/m/d H:i:s'));
			$db_games->set($id, $data);
			audit($db_audit, $id, $method, $data);
			
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		break;
	case 'gameDelete':
		$id = $_GET['id'];
		if($id) {
			$db_games->delete($id);
			audit($db_audit, $id, $method, array('deleted' => date('Y/m/d H:i:s')));
			$data = array();
			$data[0] = 1;
		}
		else {
			$data = array();
			$data[0] = 0;
		}
		break;
}

/*
elseif($method=='submitAttack') {

	$player = $_GET['player'];
	$target = $_GET['target'];
	$move = $_GET['move'];
	
	//store attack in "database"
	$gamedata->set($target, array('player' => $player, 'target' => $target));
	
	$data = array();
	$data[0] = 1;
	
}*/
/*
elseif($method=='checkAttacked') {

	$player = $_GET['player'];
	
	//lookup pending attack
	$user = $gamedata->get($player);
	if($user) {
		//$user['email']
	}
	else {
		$data = array();
		$attack['result'] = 0;
	}
}*/

if($data) {
	header('Content-Type: application/json');
	header('Cache-Control: max-age=0');
	print json_encode($data);
}
