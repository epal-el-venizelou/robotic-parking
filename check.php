<!DOCTYPE html>
<html>
<head>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f0f0f0;
        }
        .container {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
            gap: 10px;
            padding: 20px;
        }
        .square {
            background-color: #ffffff;
            border: 1px solid #ddd;
            border-radius: 10px;
            padding: 10px;
            text-align: center;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
            transition: all 0.3s ease;
            min-height: 150px;
        }
        .square:hover {
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
            transform: translateY(-5px);
        }
        .available {
            background-color: #28a745; /* Green */
            color: white;
        }
        .not-available {
            background-color: #dc3545; /* Red */
            color: white;
        }
        .square h3 {
            margin: 0;
            font-size: 18px;
            font-weight: bold;
        }
        .square p {
            margin: 5px 0;
            font-size: 16px;
        }
    </style>
</head>
<body>

<h2 style="text-align: center;">Parking Information</h2>

<div class="container">
    <?php
    // Read the content of the data.txt file
    if (file_exists('data.txt')) {
        $data = file_get_contents('data.txt');
        $lines = explode("\n", $data);
    } else {
        $lines = [];
    }

    // Array to store data with updated information
    $updated_data = [];
    $processed_ids = [];

    // Process each line from the data file
    foreach ($lines as $line) {
        $line = trim($line); // Remove any leading/trailing whitespace
        if (!empty($line)) {
            // Split the line into key-value pairs
            list($key, $value) = explode(':', $line, 2);
            $key = trim($key);
            $value = trim($value);

            // Store values for ID, Total Spaces, and Available Spaces
            if ($key == "ID") {
                $id = $value;
            }
            if ($key == "Total Spaces") {
                $totalSpaces = $value;
            }
            if ($key == "Available Spaces") {
                $availableSpaces = $value;

                // Check if the ID has already been processed
                if (in_array($id, $processed_ids)) {
                    // If ID is already processed, replace the data
                    // Find the entry for this ID and update it
                    foreach ($updated_data as $index => $entry) {
                        if ($entry['ID'] == $id) {
                            $updated_data[$index]['Total Spaces'] = $totalSpaces;
                            $updated_data[$index]['Available Spaces'] = $availableSpaces;
                        }
                    }
                } else {
                    // Otherwise, store the new data
                    $updated_data[] = [
                        'ID' => $id,
                        'Total Spaces' => $totalSpaces,
                        'Available Spaces' => $availableSpaces
                    ];
                    $processed_ids[] = $id; // Add ID to processed list
                }
            }
        }
    }

    // Write updated data back to the file
    $new_data = "";
    foreach ($updated_data as $entry) {
        $new_data .= "ID: {$entry['ID']}\n";
        $new_data .= "Total Spaces: {$entry['Total Spaces']}\n";
        $new_data .= "Available Spaces: {$entry['Available Spaces']}\n\n";
    }

    // Overwrite the file with updated data
    file_put_contents('data.txt', $new_data);

    // Display updated parking information in squares
    foreach ($updated_data as $entry) {
        $id = $entry['ID'];
        $totalSpaces = $entry['Total Spaces'];
        $availableSpaces = $entry['Available Spaces'];

        // Determine the class for available spaces (green if > 0, red if <= 0)
        $class = (intval($availableSpaces) > 0) ? 'available' : 'not-available';

        // Output the square with parking information
        echo "<div class='square $class'>";
        echo "<h3>ID: $id</h3>";
        echo "<p>Total Spaces: $totalSpaces</p>";
        echo "<p>Available Spaces: $availableSpaces</p>";
        echo "</div>";
    }
    ?>
</div>

</body>
</html>
