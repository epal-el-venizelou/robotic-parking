<!DOCTYPE html>
<html>
<body>

<?php

if (isset($_GET['id'])) {
    $id = $_GET['id'];
}
if (isset($_GET['totalSpaces'])) {
    $total = $_GET['totalSpaces'];
}
if (isset($_GET['freespaces'])) {
    $free = $_GET['freespaces'];
}


// Display numbers based on $x


// Display numbers based on $id
echo "<br> Parking Info for ID: $id <br>";
if ($id == 1) {
    echo "Different Parking for ID 1<br>";
} else {
    echo "Different Parking for other IDs<br>";
}

echo "synolikes thesis: $total <br>";
echo "athies thesis: $free <br>";

// Save data to a text file
$data = "ID: $id\nTotal Spaces: $total\nAvailable Spaces: $free\n\n";
file_put_contents('data.txt', $data, FILE_APPEND);

?>

</body>
</html>
