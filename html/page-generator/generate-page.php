<!DOCTYPE html>
<html>
<head>
    <title><?php echo $_POST['title']; ?></title>
    <style>
        body {
            background-color: <?php echo $_POST['bg-color']; ?>;
			color: <?php echo $_POST['color']; ?>;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
        }

        h1 {
            font-size: 36px;
            margin-bottom: 20px;
        }

        p {
            font-size: 18px;
            margin-bottom: 10px;
        }

		a.button {
		  display: inline-block;
		  padding: 10px 20px;
		  background-color: <?php echo $_POST['color']; ?>;
		  color: <?php echo $_POST['bg-color']; ?>;
		  text-decoration: none;
		  border-radius: 4px;
		  border: none;
		  cursor: pointer;
		  margin-top: 20px;
		}

		a.button:focus {
		  outline: none;
		}
    </style>
</head>
<body>
    <h1><?php echo $_POST['title']; ?></h1>
	<p>This page is themed with the background color: <?php echo $_POST['bg-color']; ?> and the text color <?php echo $_POST['color'] ?></p>
	<a href="index.html" class="button">Go back</a>
</body>
</html>
