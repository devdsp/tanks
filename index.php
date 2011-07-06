<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
   "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>test</title>
</head>
<body>
<h1>Testing</h1>

<?php
require('db.conf.php');
$db = new PDO('mysql:host=localhost;dbname=tanks',$db_user,$db_pass);

function unique_id($l = 4) {
    return substr(md5(uniqid(mt_rand(), true)), 0, $l);
}


class User {
    function __construct($email) {

    }

    public function Authenticate($email,$password) {
        global $db;
        if( !$db ) {
            return ("Database Unavilable");
        }
        if( $rs = $db->prepare("SELECT * FROM user WHERE email = :email")) {
            if($rs->execute(array(':email'=>$email))) {
                $row = $rs->fetch(PDO::FETCH_ASSOC);
                if( $row && sha1( $row['salt'].$password ) === $row['password'] 
                ) {
                    return true;
                } else {
                    return ("Bad username or password");
                }
            } else {
                return ("Error executing statument");
            }
        } else {
            return ("Error preparing statement");
        }
    }

    public function FindUser($email) {
        global $db;
        
        if( $rs = $db->prepare("SELECT * FROM user WHERE email = :email")) {
            if($rs->execute(array(':email'=>$email))) {
                $row = $rs->fetch(PDO::FETCH_ASSOC);
                if( $row ) {
                    return new User($email);
                } else {
                    return false;
                }
            } else {
                return "Error";
            }
        }
    }

    public function AddUser($email,$password) {
        global $db;
        if(!is_a(User::FindUser($email),'User')) {
            $rs = $db->prepare("INSERT INTO user (email,salt,password) values(?,?,?)");
            $salt = unique_id();
            $rs->execute(array($email,$salt,sha1($salt.$password)));
        };
    }
}

$rs = $db->Query('SELECT * FROM user');
print "<pre>\n".print_r($rs->fetchAll(),true)."</pre>\n";

print( (User::FindUser('adam.lloyd@gmail.com') !== false) ."<br />" );
print( User::AddUser('adam.lloyd@gmail.com','test')."<br />" );
print( (User::FindUser('adam.lloyd@gmail.com') !== false) ."<br />" );
print( User::Authenticate('adam.lloyd@gmail.com','test')."<br />" );

?>
</body>
</html>

