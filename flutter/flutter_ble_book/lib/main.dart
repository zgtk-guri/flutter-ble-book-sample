import 'package:flutter/material.dart';
import 'package:flutter_ble_book/scan.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'BLE LED Button Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

final serviceUuid = Guid("3096c932-14c0-4ac7-8372-36dce85ac7a4");
final ledCharacteristicUuid = Guid("3096c932-14c0-4ac7-8372-36dce85ac7a5");
final switchCharacteristicUuid = Guid("3096c932-14c0-4ac7-8372-36dce85ac7a6");

class _MyHomePageState extends State<MyHomePage> {
  bool _isLedOn = false;
  bool _isSwitchPushed = false;
  BluetoothDevice? _device;
  BluetoothCharacteristic? _ledCharacteristic;
  BluetoothCharacteristic? _switchCharacteristic;

  void _scanDevice() {
    setState(() {
      _device?.disconnect();
      _device = null;
    });
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => const ScanPage()),
    ).then((value) {
      if (value != null) {
        setState(() {
          _device = value;
          _device!.connect().then((_) async {
            var services = await _device!.discoverServices();
            for (var service in services) {
              if (service.uuid == serviceUuid) {
                for (var characteristic in service.characteristics) {
                  if (characteristic.uuid == ledCharacteristicUuid) {
                    _ledCharacteristic = characteristic;
                  } else if (characteristic.uuid == switchCharacteristicUuid) {
                    _switchCharacteristic = characteristic;
                    _switchCharacteristic!.setNotifyValue(true);
                    _switchCharacteristic!.onValueReceived.listen((event) {
                      setState(() {
                        _isSwitchPushed = event[0] == 1;
                      });
                    });
                  }
                }
              }
            }
          });
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text('BLE LED Button Demo'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Text('Device: '),
                Text(
                    _device == null
                        ? 'Not connected'
                        : 'Connected to ${_device!.platformName}',
                    style: Theme.of(context).textTheme.headlineMedium),
              ],
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Text('LED: '),
                Switch(
                  value: _isLedOn,
                  onChanged: (bool value) {
                    setState(() {
                      _isLedOn = value;
                      _ledCharacteristic?.write([value ? 1 : 0]);
                    });
                  },
                ),
              ],
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Text('Switch: '),
                Text(_isSwitchPushed ? 'Pushed' : 'Released',
                    style: Theme.of(context).textTheme.headlineMedium)
              ],
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _scanDevice,
        tooltip: 'Increment',
        child: const Text('Scan'),
      ),
    );
  }
}
