<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Online Judge</title>
</head>
<body>
<h1>Online Judge</h1>
<h2>Problem: Test</h2>
<h3>Problem Description</h3>
<p>Write a program to calculate A + B.</p>
<h3>Input</h3>
<p>Each line with two number A and B.</p>
<h3>Output</h3>
<p>Each line with A + B.</p>
<h3>Sample Input</h3>
<p>3 4</p>
<p>5 5</p>
<h3>Sample Output</h3>
<p>7</p>
<p>10</p>
<form action="" method="post">
    <h3>Code</h3>
    <label for="code"></label>
    <textarea id="code" name="code"></textarea>
    <br/>
    <label for="type"></label>
    <select id="type" name="type">
        <option value="c">C</option>
        <option value="c++">C++</option>
    </select>
    <button type="submit" id="submit">Submit</button>
    <button type="reset">Cancel</button>
</form>
<?php
$types = ['c', 'c++'];
if (isset($_POST['code']) && isset($_POST['type'])) {
    ?>
    <h3>Result</h3>
    <?php
    if (in_array($_POST['code'], $types)) {
        echo 'Source type error!';
    } else {
        $id = rand(5, 100);
        $name = 'examples/test';
        $type = $_POST['type'] == 'c' ? 'c' : 'cxx';
        file_put_contents("../$name" . "_$id.$type", $_POST['code']);
        try {
            $context = new ZMQContext();
            $request = new ZMQSocket($context, ZMQ::SOCKET_REQ);
            $request->connect("tcp://localhost:5555");
            $request->send("$id:$name:$type");
            $reply = explode(":", $request->recv());
        } catch (ZMQSocketException $e) {
            $exception = 'ZeroMQ exception!';
        }
        unlink("../$name" . "_$id.$type");
    }
}
if (isset($reply)) {
    ?>
    <p><?= $reply[1] ?></p>
    <?php
    if ($reply[1] === 'Accepted!') {
        ?>
        <h3>Program Usage</h3>
        <p>Compile time: <?= $reply[2] ?></p>
        <p>Execution time: <?= $reply[3] ?></p>
        <p>Memory Usage: <?= $reply[4] ?></p>
        <?php
    }
}
?>
</body>
</html>