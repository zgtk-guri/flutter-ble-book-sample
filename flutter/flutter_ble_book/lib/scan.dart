import 'dart:async';
import 'dart:developer';

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

class ScanPage extends StatefulWidget {
  const ScanPage({Key? key}) : super(key: key);

  @override
  State<StatefulWidget> createState() => new ScanPageState();
} 

class ScanPageState extends State<ScanPage> {
  final List<BluetoothDevice> deviceList = <BluetoothDevice>[];
  late StreamSubscription<List<ScanResult>> scanSubscription;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: const Text('BLE Device Scan'),
      ),
      body: SafeArea(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.end,
          children: <Widget>[
            Expanded(
              child: ListView.builder(
                itemCount: deviceList.length,
                itemBuilder: (BuildContext context, int index) {
                  final BluetoothDevice device = deviceList[index];
                  return ListTile(
                    title: Text(device.platformName),
                    subtitle: Text(device.remoteId.toString()),
                    onTap: () {
                      Navigator.pop(context, device);
                    },
                  );
                },
              ),
            ),
          ],
        ),
      ),
    );
  }

  @override
  void initState() {
    super.initState();

    scanSubscription =
        FlutterBluePlus.scanResults.listen((List<ScanResult> results) {
      for (ScanResult result in results) {
        if (!deviceList.contains(result.device)) {
          log('Device found: ${result.device.platformName}, ${result.device.remoteId}');
          if (mounted) {
            setState(() {
              deviceList.add(result.device);
            });
          }
        }
      }
    });
    FlutterBluePlus.adapterState
        .where((state) => state == BluetoothAdapterState.on)
        .first
        .then((_) {
      FlutterBluePlus.startScan(
          withServices: [Guid("3096c932-14c0-4ac7-8372-36dce85ac7a4")]);
    });
  }

  @override
  void dispose() {
    super.dispose();
    FlutterBluePlus.stopScan();
    scanSubscription.cancel();
  }
}
