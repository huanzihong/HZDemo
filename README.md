# HZDEMO

## 遇到的问题：

**1.当僵尸的MoveTargetFragment的Center设置为玩家位置时，玩家起跳，怪物z方向也会跟着位移**

**原因**:怪物是最后是通过SetLocation设置为MoveTarget的Location 所以玩家起跳Z也会跟着变

**2.如果把MoveTarget设置为玩家位置在前进的途中会直接穿过障碍物**

**原因**:使用ISM表示的怪物没有碰撞

**问题1和2的解决办法是**：接入MassNavMesh怪物会根据Navmesh寻路，这样就会绕过障碍物，并且MoveTarget的位置也会沿着可行走表面去设置

**3.生成5000个Entity 只有30帧？**

**原因**:通过Unrealinsights 查看到AvoidanceProcessor用了大概16ms，这显然是不合理的。查看AvoidanceProcesser源码发现，它会通过Tag去过滤怪物，而Tag的添加需要SimulationLodTrait，但是怪物没有添加SimulationLodTrait，没有添加导致没有被过滤 所有5000个Entity都在执行避障

**解决办法**:添加后SimulationLodProcessor会去更新Tag，AvoidanceProcessor会根据Tag进行过滤，添加后5000个实体稳定60帧

**4.怪物不动了 没有根据StateTree去执行相应的逻辑**

**原因**:打断点发现StateTree的调用栈中的Tick一定会在收到Signal之后触发，StateTree只有收到信号后才会去Tick，才会去触发状态的转换

**解决办法**:在合适的Processor去SignalEntity

**5.指定MoveCenter之后 怪物到达点之后还会继续往前位移一段距离**

**解决办法**:MovingSteeringParams的ReactionTime调小

**6.客户端上生成50个Entity但是却只能看到1个**

**原因**:MoveTargetFragment没有被同步  全部都被设置成0 导致所有的Entity重叠在一块

**解决办法**:同步MoveTargetFragment











