<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"> 
<html>
<head>
<script src="linenumbers.js"></script>
<link rel="stylesheet" href="kokompe.css">
<title>Kokompe IDE on PHMGRID1</title>
</head>
<body>

<?php

// DEFINES
$model = $_POST['model'];
$do = $_POST['do'];
$cad_savecontent = stripslashes($_POST['cad_savecontent']);
$thispage = $_SERVER['PHP_SELF'];
$dot_cad = "CAD/$model.cad";
$dot_py = "CAD/$model.py";
$dot_math = "MATH/$model.math";
$dot_postfix = "MATH/$model.postfix";
$dot_stl = "STL/$model.stl";
$dot_jnlp = "JNLP/$model.jnlp";

// FUNCTIONS

//-----
function python_to_math ($dot_cad) {
  global $dot_py;
  $python_string = "
from string import *
from math import *
class dummy_cad():
  def __init__(self):
    return
cad=dummy_cad()
execfile (\"$dot_cad\")
print cad.function";
  file_put_contents($dot_py, $python_string);
  $math_string = shell_exec("python $dot_py");
  return $math_string;
}

//---------------------------
function make_jnlp ($model) {
  global $dot_stl, $dot_jnlp;
  $url_root = "http://phmgrid1.media.mit.edu/kokompe/";
  $url_stl = $url_root.$dot_stl;
  $url_jnlp = $url_root.$dot_jnlp;
  if (!$fp = fopen ( $dot_jnlp, "w" ) ) {
    print "unable to open ".$file_jnlp;
    print "<br>I share your inconsolable sadness.";
    exit;
  }
  fwrite($fp, "
  <?xml version=\"1.0\" encoding=\"utf-8\"?>
  <jnlp codebase=\"http://phmgrid1.media.mit.edu/kokompe/\" href=\"$dot_jnlp\">
    <information>
      <title>KOKOMPE Project 3D Viewer</title>
      <vendor>Massachusetts Institute of Technology</vendor>
      <homepage href=\"http://phmgrid1.media.mit.edu/kokompe\"/>
      <description>KOKOMPE Spinny-Viewer</description>
      <description kind=\"short\">KOKOMPE Viewer</description>
      <offline-allowed/>
    </information>
    <resources>
      <j2se href=\"http://java.sun.com/products/autodl/j2se\" version=\"1.4+\"/>
      <property name=\"sun.java2d.noddraw\" value=\"true\"/>
      <property name=\"jnlp.stlurl\" value=\"".$url_stl."\"/> 
      <jar href=\"javaviewer.jar\" main=\"true\"/>
      <extension name=\"jogl\" href=\"http://download.java.net/media/jogl/builds/archive/jsr-231-webstart-current/jogl.jnlp\" />
    </resources>
  <application-desc main-class=\"javaviewer.Viewer\"></application-desc>
  </jnlp>");
  fclose($fp);
}

//===========================================
// BEGIN LOGIC  
// (what happens when buttons pressed)
//===========================================

// SAVE TEXT BOX
switch ($do) {
  case "SAVE FILE":
    file_put_contents($dot_cad, $cad_savecontent);
    break;
  
  case "SAVE & RENDER":
    file_put_contents($dot_cad, $cad_savecontent);
    $math_string = python_to_math ($dot_cad);
    file_put_contents($dot_math, $math_string);
    $postfix = shell_exec("infix_to_postfix < $dot_math");
    file_put_contents($dot_postfix, $postfix);
    $rendres = $_POST['rendres'];
    $rendlevl = $_POST['rendlevl'];
    $rendboxl = $_POST['rendboxl'];
    $rendboxhl = $rendboxl/2;
    shell_exec("math_string_to_stl -$rendboxhl $rendboxhl -$rendboxhl $rendboxhl -$rendboxhl $rendboxhl $rendres $rendlevl < $dot_postfix > $dot_stl");
    make_jnlp( $model );
    break;
}

// LOAD CAD FILE INTO TEXT BOX  (do always) 
$cad_loadcontent = htmlspecialchars(file_get_contents($dot_cad));

?>

<!========================== 
  BEGIN HTML 
  (draw buttons, edit area)
 =========================== -->

<form name=editwindow method=post action="<?=$thispage?>">

<div class=titlebar>
<input class=title type=text size=10 name=model value=<?=$model?>> :: 
<a href=CAD/<?=$model?>.cad>.cad</a> |
<a href=MATH/<?=$model?>.math>.math</a> |
<a href=MATH/<?=$model?>.postfix>.postfix</a> |
<a href=STL/<?=$model?>.stl>.stl</a> |
<a href=JNLP/<?=$model?>.jnlp>.jnlp</a> 
</div>

<!-- TOOLBAR -->
<div class="toolbar"> 
<input class=button type="submit" name="do" value="LOAD FILE">
<input class=button type="submit" name="do" value="SAVE FILE">
<input class=button type="submit" name="do" value="SAVE & RENDER">
<br>
Resolution: <input class=field type=text size=1 name=rendres value="0.03">
&nbsp; &nbsp;
Render level:
<input type=radio name=rendlevl value=0 checked=checked>Zero
<input type=radio name=rendlevl value=1>One
<input type=radio name=rendlevl value=2>Two
<br>Render bounding box max dimension: <input class=field type=text size=1 name=rendboxl value="2">
</div>

<!-- TEXT EDITOR AREA -->
<span class=editarea>
<textarea class="cad" id="cad_savecontent" name="cad_savecontent" cols=70 rows=33><?=$cad_loadcontent?></textarea>
<script type="text/javascript">createTextAreaWithLines('cad_savecontent');</script> 
</form>
</span>

<!-- FOOTER -->
<div class=footer>
<hr>
<span class=right>27 Dec 2007</span> A. Sun 
</div>
</body>
</html>
