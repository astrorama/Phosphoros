<?php

    function compare_version($v1,$v2){
    // return 1 if v1>v2, 0 if v1=v2, -1 if v1<v2
        $v1_c = explode('.',$v1);
        $v2_c = explode('.',$v2);
        
        if ($v1_c[0] > $v2_c[0]) {
            return 1;
        } else if ($v1_c[0] < $v2_c[0]) {
            return -1;
        } else {
            if ($v1_c[1] > $v2_c[1]) {
                return 1;
            } else if ($v1_c[1] < $v2_c[1]) {
                return -1;
            } else {
                return 0;
            }
        }
    }

	$directory = './Data/';
	
	if (isset($_GET['version'])){
	    $file_name = 'AuxiliaryData_'.$_GET['version'].'.tar.xz';
	    $file = $directory.$file_name;
	    if (file_exists($file)) {
            header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
            header("Cache-Control: public"); // needed for internet explorer
            header("Content-Type: application/zip");
            header("Content-Transfer-Encoding: Binary");
            header("Content-Length:".filesize($file));
            header("Content-Disposition: attachment; filename=".$file_name);
            readfile($file);
            die();        
	    } else {
	        echo "Error: Invalid version"; 
	        exit ();
	    }
	} else {
	    $last_version ='0.0';
	    foreach(scandir($directory) as $file) {
	        if ( strpos($file,'AuxiliaryData_') !== false && strpos($file,'.tar.xz') !== false ){
	            $version = str_replace('.tar.xz', '', explode ('_' , $file )[1]);
	            if (compare_version($version, $last_version)==1) {
	                $last_version = $version;
	            } 
	       }
	    }   
	
	    
	    echo $last_version;
	}
	
?>
