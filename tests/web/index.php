<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Online Judge</title>
    <link rel="stylesheet" type="text/css" href="index.css">
</head>
<body>
<header>
    <h1><a href="">Online Judge</a></h1>
    <hr/>
</header>
<main>
    <h2>Problem: Example</h2>
    <div id="wrapper">
        <div id="description">
            <h3>Problem Description</h3>
            <em>Write a program to calculate A + B.</em>
            <h3>Input</h3>
            <em>Each line with two number A and B.</em>
            <h3>Output</h3>
            <em>Each line with A + B.</em>
            <h3>Sample Input</h3>
            <em>3 4</em><br/>
            <em>5 5</em>
            <h3>Sample Output</h3>
            <em>7</em><br/>
            <em>10</em>
        </div>
        <div id="solution">
            <form action="" method="post">
                <fieldset>
                    <legend><strong>Code Section</strong></legend>
                    <label for="code">Input your code here</label>
                    <textarea id="code" name="code" cols="50" rows="15"></textarea>
                    <label for="type">Select source code type</label>
                    <select id="type" name="type">
                        <option value="c">C</option>
                        <option value="c++">C++</option>
                    </select>
                    <br/>
                    <div id="button-group">
                        <button type="submit" id="submit">Submit</button>
                        <button type="reset">Cancel</button>
                    </div>
                </fieldset>
            </form>
        </div>
    </div>
    <div id="result">
        <?php
        $types = ['c', 'c++'];
        if (isset($_POST['code']) && isset($_POST['type'])) {
            if (in_array($_POST['code'], $types)) {
                echo 'Source type error!';
            } else {
                $id = rand(5, 100);
                $io = 'examples/example';
                $code = 'tests/test';
                $type = $_POST['type'] == 'c' ? 'c' : 'cxx';
                file_put_contents("../../$code" . "_$id.$type", $_POST['code']);
                try {
                    $context = new ZMQContext();
                    $request = new ZMQSocket($context, ZMQ::SOCKET_REQ);
                    $request->connect("tcp://localhost:5555");
                    $request->send("$id:$io:$code:$type");
                    $reply = explode(":", $request->recv());
                } catch (ZMQSocketException $e) {
                    $exception = 'ZeroMQ exception!';
                }
                unlink("../../$code" . "_$id.$type");
            }
        }
        if (isset($reply[1]) && $reply[1] === 'Accepted!') {
            ?>
            <h3>Result & Usage</h3>
            <strong><?= $reply[1] ?></strong><br/>
            <em>Compile time: <?= $reply[2] ?></em><br/>
            <em>Execution time: <?= $reply[3] ?></em><br/>
            <em>Memory Usage: <?= $reply[4] ?></em><br/>
            <?php
        } else if (isset($reply[1]) && $reply[1] !== "Accepted!") {
            ?>
            <h3>Result</h3>
            <strong><?= $reply[1] ?></strong>
            <?php
        }
        ?>
    </div>
</main>
</body>
</html>
