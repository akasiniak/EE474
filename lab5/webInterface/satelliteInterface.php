<?php
if(file_exists($_POST['file'])){
    $file = fopen($_POST['File'], "a");
    
    $data = $_POST["command"];
    
    fwrite($file, $data);
    fclose($file);
}
?>